
/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRLayer.
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center
 *(DLR) Oliver Böttcher, oliver.boettcher@dlr.de, German Aerospace Center (DLR)
 *			 Cristhian Eduardo Murcia Galeano,
 *cristhianmurcia182@gmail.com Ana Maria Orozco, ana.orozco.net@gmail.com
 *
 ******************************************************************************
 * Copyright 2022 German Aerospace Center (DLR), Institute of Transportation
 *Systems
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

#include "cpl_error.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
using namespace odr;
using namespace std;

const std::map<XODRLayerType, std::string> OGRXODRLayer::layerTypeToString = {
    {XODRLayerType::ReferenceLine, "ReferenceLine"},
    {XODRLayerType::LaneBorder, "LaneBorder"},
    {XODRLayerType::RoadMark, "RoadMark"},
    {XODRLayerType::RoadObject, "RoadObject"},
    {XODRLayerType::Lane, "Lane"}};

OGRXODRLayer::OGRXODRLayer(VSILFILE *filePtr, XODRLayerType xodrLayerType,
                           std::vector<odr::Road> xodrRoads, std::string proj4Defn): 
    file(filePtr),
    layerType(xodrLayerType),
    roads(xodrRoads),
    spatialRef(nullptr)
{
    roadElements = createRoadElements();

    std::string layerName = layerTypeToString.at(layerType);
    this->featureDefn = new OGRFeatureDefn(layerName.c_str());
    SetDescription(featureDefn->GetName());
    featureDefn->Reference(); // TODO Why calling this? A call to Dereference() is required as well then.
    ResetReading();

    spatialRef = new OGRSpatialReference();
    spatialRef->importFromProj4(proj4Defn.c_str());
    featureDefn->GetGeomFieldDefn(0)->SetSpatialRef(spatialRef);

    defineFeatureClass();
}

OGRXODRLayer::~OGRXODRLayer()
{
    if (featureDefn)
    {
        featureDefn->Release();
    }

    if (spatialRef)
    {
        spatialRef->Release();
    }  
}

OGRFeature *OGRXODRLayer::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (layerType == XODRLayerType::ReferenceLine)
    {
        if (roadIter != roads.end())
        {
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            std::unique_ptr<OGRLineString> lineString(new OGRLineString());

            const double eps = 0.9;
            odr::Road road = *roadIter;
            odr::RefLine refLine = road.ref_line;
            std::set<double> sVals = refLine.approximate_linear(eps, 0.0, road.length);

            for (const double &s : sVals)
            {
                odr::Vec3D roadGeometry = refLine.get_xyz(s);
                lineString->addPoint(roadGeometry[0], roadGeometry[1]);
            }

            std::unique_ptr<OGRGeometry> geometry(lineString->MakeValid());
            feature->SetGeometry(geometry.get());
            feature->SetField("ID", road.id.c_str());
            feature->SetField("Length", road.length);
            feature->SetField("Junction", road.junction.c_str());
            feature->SetFID(nNextFID++);

            roadIter++;
        }
    }
    else if (layerType == XODRLayerType::LaneBorder)
    {
        if (laneIter != roadElements.lanes.end())
        {
            odr::Lane lane = *laneIter;
            odr::Line3D laneOuter = *laneLinesOuterIter;
            std::string laneRoadID = *laneRoadIDIter;
            
            OGRLineString lineString;

            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

            for(auto laneOuterIter = laneOuter.begin(); laneOuterIter != laneOuter.end(); ++laneOuterIter) {
                odr::Vec3D laneVertex = *laneOuterIter;
                lineString.addPoint(laneVertex[0], laneVertex[1]);
            }

            std::unique_ptr<OGRGeometry> geometry(lineString.MakeValid());
            feature->SetGeometry(geometry.get());
            feature->SetField(featureDefn->GetFieldIndex("RoadID"), laneRoadID.c_str());
            feature->SetField(featureDefn->GetFieldIndex("ID"), lane.id);
            feature->SetField(featureDefn->GetFieldIndex("Type"), lane.type.c_str());
            feature->SetField(featureDefn->GetFieldIndex("Predecessor"), lane.predecessor);
            feature->SetField(featureDefn->GetFieldIndex("Successor"), lane.successor);
            feature->SetFID(nNextFID++);

            laneIter++;
            laneLinesOuterIter++;
            laneLinesInnerIter++; // For consistency, even though not used here
            laneRoadIDIter++;
        }
    }
    else if (layerType == XODRLayerType::RoadMark)
    {
        if (roadMarkIter != roadElements.roadMarks.end())
        {
            odr::RoadMark roadMark = *roadMarkIter;        
            odr::Mesh3D roadMarkMesh = *roadMarkMeshIter;
            
            OGRLineString lineStringEven;
            OGRLineString lineStringOdd;

            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

            std::vector<odr::Vec3D> roadMarkVertices = roadMarkMesh.vertices;
            for (std::size_t iVertex = 0; iVertex < roadMarkVertices.size(); iVertex++)
            {
                odr::Vec3D roadMarkVertex = roadMarkVertices[iVertex];
                if (iVertex % 2 != 0)
                {
                    lineStringEven.addPoint(roadMarkVertex[0], roadMarkVertex[1]);
                }
                else
                {
                    lineStringOdd.addPoint(roadMarkVertex[0], roadMarkVertex[1]);
                }
            }

            OGRLinearRing ring;
            ring.addSubLineString(&lineStringOdd);
            lineStringEven.reversePoints();
            ring.addSubLineString(&lineStringEven);
            // Close the ring
            OGRPoint startPoint;
            ring.getPoint(0, &startPoint);
            ring.addPoint(startPoint.getX(), startPoint.getY());

            std::unique_ptr<OGRPolygon> polygon(new OGRPolygon());
            polygon->addRing(&ring);
            std::unique_ptr<OGRGeometry> geometry(polygon->MakeValid());

            // TODO move out to separate function and combine with lane and road object polygon check
            std::string geomType = geometry->getGeometryName();
            if (strcmp(geomType.c_str(), "POLYGON") != 0) {
                // Function MakeValid() destroyed the initial Polygon
                // TODO Filter out dangling parts
                // TODO Use OGRGeometryFactory::removeLowerDimensionSubGeoms at first
                std::printf("%s::%d: Geometry of road mark is supposed to be POLYGON but is this instead: %s\n", 
                            __FILE__, __LINE__, geometry->exportToWkt().c_str());
            }

            feature->SetGeometry(geometry.get());
            feature->SetField(featureDefn->GetFieldIndex("RoadID"), roadMark.road_id.c_str());
            feature->SetField(featureDefn->GetFieldIndex("LaneID"), roadMark.lane_id);
            feature->SetField(featureDefn->GetFieldIndex("Type"), roadMark.type.c_str());
            feature->SetFID(nNextFID++);

            roadMarkIter++;
            roadMarkMeshIter++;
        }
    }
    else if (layerType == XODRLayerType::RoadObject)
    {
        if (roadObjectIter != roadElements.roadObjects.end())
        {
            odr::RoadObject roadObject = *roadObjectIter;
            const odr::Mesh3D &roadObjectMesh = *roadObjectMeshesIter;
            const std::vector<odr::Vec3D> roadObjectVertices = roadObjectMesh.vertices;

            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            std::unique_ptr<OGRLineString> lineString(new OGRLineString());

            for (std::size_t iVertex = 0; iVertex < roadObjectVertices.size(); iVertex++)
            {
                const odr::Vec3D &roadObjectVertex = roadObjectVertices[iVertex];
                lineString->addPoint(roadObjectVertex[0], roadObjectVertex[1]);
            }
            std::unique_ptr<OGRGeometry> geometry(lineString->MakeValid());
            feature->SetGeometry(geometry.get());
            feature->SetField(featureDefn->GetFieldIndex("ObjectID"), roadObject.id.c_str());
            feature->SetField(featureDefn->GetFieldIndex("RoadID"), roadObject.road_id.c_str());
            feature->SetField(featureDefn->GetFieldIndex("Type"), roadObject.type.c_str());
            feature->SetField(featureDefn->GetFieldIndex("Name"), roadObject.name.c_str());
            feature->SetFID(nNextFID++);

            roadObjectIter++;
            roadObjectMeshesIter++;
        }
    }
    else if (layerType == XODRLayerType::Lane){

        while(laneIter != roadElements.lanes.end() && (*laneIter).id == 0)
        {
            // Skip lane(s) with id 0
            laneIter++;
            laneLinesOuterIter++;
            laneLinesInnerIter++;
            laneRoadIDIter++;
        }      

        if(laneIter != roadElements.lanes.end()) {
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            
            odr::Line3D laneInner = *laneLinesInnerIter;
            odr::Line3D laneOuter = *laneLinesOuterIter;
            odr::Lane lane = *laneIter;
            std::string laneRoadID = *laneRoadIDIter;

            OGRLinearRing ring;

            for(auto laneOuterIter = laneOuter.begin(); laneOuterIter != laneOuter.end(); ++laneOuterIter) {
                odr::Vec3D laneVertex = *laneOuterIter;
                ring.addPoint(laneVertex[0], laneVertex[1]);
            }

            for(auto laneInnerIter = laneInner.rbegin(); laneInnerIter != laneInner.rend(); ++laneInnerIter) {
                odr::Vec3D laneVertex = *laneInnerIter;
                ring.addPoint(laneVertex[0], laneVertex[1]);
            }

            // Close the ring
            OGRPoint startPoint;
            ring.getPoint(0, &startPoint);
            ring.addPoint(startPoint.getX(), startPoint.getY()); // TODO change everywhere to ring.addPoint(&startPoint);
           
            std::unique_ptr<OGRPolygon> polygon(new OGRPolygon());
            polygon->addRing(&ring);
            std::unique_ptr<OGRGeometry> geometry(polygon->MakeValid());
                        
            std::string geomType = geometry->getGeometryName();
            if (strcmp(geomType.c_str(), "POLYGON") != 0) {
                // Function MakeValid() destroyed the initial Polygon
                // TODO Filter out dangling parts
                // TODO Use OGRGeometryFactory::removeLowerDimensionSubGeoms at first
                std::printf("%s::%d: Geometry of road(%s)::lane(%d) is supposed to be POLYGON but is this instead: %s\n", 
                            __FILE__, __LINE__, laneRoadID.c_str(), lane.id, geometry->exportToWkt().c_str());
            }

            feature->SetGeometry(geometry.get());

            feature->SetFID(nNextFID++);
            feature->SetField(featureDefn->GetFieldIndex("RoadID"), laneRoadID.c_str());
            feature->SetField(featureDefn->GetFieldIndex("LaneID"), lane.id);
            feature->SetField(featureDefn->GetFieldIndex("Type"), lane.type.c_str());
            feature->SetField(featureDefn->GetFieldIndex("Predecessor"), lane.predecessor);
            feature->SetField(featureDefn->GetFieldIndex("Successor"), lane.successor);
            
            laneIter++;
            laneLinesOuterIter++;
            laneLinesInnerIter++;
            laneRoadIDIter++;
        }
    } 

    if (feature)
    {
        return feature.release();
    }
    else
    {
        // End of features for the given layer reached.
        return nullptr;
    }
}

void OGRXODRLayer::ResetReading()
{
    VSIFSeekL(file, 0, SEEK_SET);
    nNextFID = 0;
    resetRoadElementIterators();
}

void OGRXODRLayer::defineFeatureClass()
{
    if (layerType == XODRLayerType::ReferenceLine)
    {
        featureDefn->SetGeomType(wkbLineString);
        
        OGRFieldDefn oFieldID("ID", OFTString);
        featureDefn->AddFieldDefn(&oFieldID);

        OGRFieldDefn oFieldLen("Length", OFTReal);
        featureDefn->AddFieldDefn(&oFieldLen);

        OGRFieldDefn oFieldJunction("Junction", OFTString);
        featureDefn->AddFieldDefn(&oFieldJunction);
    }
    else if (layerType == XODRLayerType::LaneBorder)
    {
        featureDefn->SetGeomType(wkbLineString);

        OGRFieldDefn oFieldID("ID", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldID);

        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);

        OGRFieldDefn oFieldPred("Predecessor", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldPred);

        OGRFieldDefn oFieldSuc("Successor", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldSuc);
    }
    else if (layerType == XODRLayerType::RoadMark)
    {
        featureDefn->SetGeomType(wkbPolygon);

        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldLaneID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);
    }
    else if (layerType == XODRLayerType::RoadObject)
    {
        featureDefn->SetGeomType(wkbLineString);

        OGRFieldDefn oFieldObjectID("ObjectID", OFTString);
        featureDefn->AddFieldDefn(&oFieldObjectID);

        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);

        OGRFieldDefn oFieldObjectName("Name", OFTString);
        featureDefn->AddFieldDefn(&oFieldObjectName);
    }
    else if (layerType == XODRLayerType::Lane)
    {
        featureDefn->SetGeomType(wkbPolygon);

        OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldLaneID);

        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);

        OGRFieldDefn oFieldPred("Predecessor", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldPred);

        OGRFieldDefn oFieldSuc("Successor", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldSuc);
    }
}

void OGRXODRLayer::resetRoadElementIterators()
{
    roadIter = roads.begin();

    laneIter = roadElements.lanes.begin();
    laneSectionIter = roadElements.laneSections.begin();
    laneRoadIDIter = roadElements.laneRoadIDs.begin();
    laneMeshIter = roadElements.laneMeshes.begin();

    laneLinesInnerIter = roadElements.laneLinesInner.begin();
    laneLinesOuterIter = roadElements.laneLinesOuter.begin();

    roadMarkIter = roadElements.roadMarks.begin();
    roadMarkMeshIter = roadElements.roadMarkMeshes.begin();

    roadObjectIter = roadElements.roadObjects.begin();
    roadObjectMeshesIter = roadElements.roadObjectMeshes.begin();
}

OGRXODRLayer::RoadElements OGRXODRLayer::createRoadElements(const double eps)
{
    RoadElements elements;

    for (odr::Road road : roads)
    {
        for (odr::LaneSection laneSection : road.get_lanesections())
        {
            elements.laneSections.push_back(laneSection);

            for (odr::Lane lane : laneSection.get_lanes())
            {
                elements.laneRoadIDs.push_back(road.id);

                elements.lanes.push_back(lane);

                odr::Mesh3D laneMesh = road.get_lane_mesh(lane, eps);
                elements.laneMeshes.push_back(laneMesh);

                odr::Line3D laneLineOuter = road.get_lane_border_line(lane, eps, true);
                elements.laneLinesOuter.push_back(laneLineOuter);
                
                odr::Line3D laneLineInner = road.get_lane_border_line(lane, eps, false);
                elements.laneLinesInner.push_back(laneLineInner);

                for (odr::RoadMark roadMark : lane.get_roadmarks(
                         laneSection.s0, road.get_lanesection_end(laneSection)))
                {
                    elements.roadMarks.push_back(roadMark);

                    odr::Mesh3D roadMarkMesh = road.get_roadmark_mesh(lane, roadMark, eps);
                    elements.roadMarkMeshes.push_back(roadMarkMesh);
                }
            }
        }

        for (odr::RoadObject roadObject : road.get_road_objects())
        {
            elements.roadObjects.push_back(roadObject);

            odr::Mesh3D roadObjectMesh =
                road.get_road_object_mesh(roadObject, eps);
            elements.roadObjectMeshes.push_back(roadObjectMesh);
        }
    }
    return elements;
}