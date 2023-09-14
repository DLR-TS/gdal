
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
                           std::vector<odr::Road> xodrRoads, std::string proj4Defn,
                           bool dissolveTriangulatedSurface): 
    file(filePtr),
    layerType(xodrLayerType),
    roads(xodrRoads),
    spatialRef(nullptr),
    dissolveSurface(dissolveTriangulatedSurface)
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
                odr::Vec3D refLineVertex = refLine.get_xyz(s);
                lineString->addPoint(refLineVertex[0], refLineVertex[1], refLineVertex[2]);
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
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

            odr::Lane lane = *laneIter;
            odr::Line3D laneOuter = *laneLinesOuterIter;
            std::string laneRoadID = *laneRoadIDIter;
            
            OGRLineString lineString;

            for(auto laneOuterIter = laneOuter.begin(); laneOuterIter != laneOuter.end(); ++laneOuterIter) {
                odr::Vec3D laneVertex = *laneOuterIter;
                lineString.addPoint(laneVertex[0], laneVertex[1], laneVertex[2]);
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
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            
            odr::RoadMark roadMark = *roadMarkIter;        
            odr::Mesh3D roadMarkMesh = *roadMarkMeshIter;
            
            OGRTriangulatedSurface tin = triangulateSurface(roadMarkMesh);

            if (dissolveSurface) {
                OGRGeometry* dissolvedPolygon = tin.UnaryUnion();
                bool isSimple = dissolvedPolygon->IsSimple();
                if(!isSimple) {
                    CPLError(CE_Warning, CPLE_WrongFormat,
                             "Dissolved road mark polygon of road(%s):lane(%d) is not simple",
                             roadMark.road_id.c_str(), roadMark.lane_id);
                }
                feature->SetGeometry(dissolvedPolygon);
            } else {
                //tin.MakeValid(); // TODO Works for TINs only with enabled SFCGAL support
                feature->SetGeometry(&tin);
            }
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
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            
            odr::RoadObject roadObject = *roadObjectIter;
            odr::Mesh3D roadObjectMesh = *roadObjectMeshesIter;

            OGRTriangulatedSurface tin = triangulateSurface(roadObjectMesh);

            if (dissolveSurface) {
                OGRGeometry* dissolvedPolygon = tin.UnaryUnion();
                bool isSimple = dissolvedPolygon->IsSimple();
                if(!isSimple) {
                    CPLError(CE_Warning, CPLE_WrongFormat,
                             "Dissolved road object polygon of road(%s):object(%s) is not simple",
                             roadObject.road_id.c_str(), roadObject.id.c_str());
                }
                feature->SetGeometry(dissolvedPolygon);
            } else {
                //tin.MakeValid(); // TODO Works for TINs only with enabled SFCGAL support
                feature->SetGeometry(&tin);
            }

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
            laneMeshIter++;
            laneRoadIDIter++;
        }      

        if(laneIter != roadElements.lanes.end()) {
            feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));
            
            odr::Lane lane = *laneIter;
            odr::Mesh3D laneMesh = *laneMeshIter;

            std::string laneRoadID = *laneRoadIDIter;

            OGRTriangulatedSurface tin = triangulateSurface(laneMesh);

            if (dissolveSurface) {
                OGRGeometry* dissolvedPolygon = tin.UnaryUnion();
                bool isSimple = dissolvedPolygon->IsSimple();
                if(!isSimple) {
                    CPLError(CE_Warning, CPLE_WrongFormat,
                             "Dissolved lane polygon of road(%s):lane(%d) is not simple",
                             laneRoadID.c_str(), lane.id);
                }
                feature->SetGeometry(dissolvedPolygon);
            } else {
                //tin.MakeValid(); // TODO Works for TINs only with enabled SFCGAL support
                feature->SetGeometry(&tin);
            }

            feature->SetFID(nNextFID++);
            feature->SetField(featureDefn->GetFieldIndex("RoadID"), laneRoadID.c_str());
            feature->SetField(featureDefn->GetFieldIndex("LaneID"), lane.id);
            feature->SetField(featureDefn->GetFieldIndex("Type"), lane.type.c_str());
            feature->SetField(featureDefn->GetFieldIndex("Predecessor"), lane.predecessor);
            feature->SetField(featureDefn->GetFieldIndex("Successor"), lane.successor);
            
            laneIter++;
            laneMeshIter++;
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
        if (dissolveSurface) {
            featureDefn->SetGeomType(wkbPolygon);  
        } else {
            featureDefn->SetGeomType(wkbTINZ);
        }
        
        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldLaneID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);
    }
    else if (layerType == XODRLayerType::RoadObject)
    {
        if (dissolveSurface) {
            featureDefn->SetGeomType(wkbPolygon);  
        } else {
            featureDefn->SetGeomType(wkbTINZ);
        }

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
        if (dissolveSurface) {
            featureDefn->SetGeomType(wkbPolygon);  
        } else {
            featureDefn->SetGeomType(wkbTINZ);
        }

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

OGRTriangulatedSurface OGRXODRLayer::triangulateSurface(odr::Mesh3D mesh) {
    std::vector<odr::Vec3D> meshVertices = mesh.vertices;
    std::vector<uint32_t> meshIndices = mesh.indices;

    OGRTriangulatedSurface tin;
    const size_t numIndices = meshIndices.size();
    // Build triangles from mesh vertices.
    // Each triple of mesh indices defines which vertices form a triangle.
    for (std::size_t idx = 0; idx < numIndices; idx += 3) {
        uint32_t vertexIdx = meshIndices[idx];
        odr::Vec3D vertexP = meshVertices[vertexIdx];
        OGRPoint p(vertexP[0], vertexP[1], vertexP[2]);

        vertexIdx = meshIndices[idx + 1];
        odr::Vec3D vertexQ = meshVertices[vertexIdx];
        OGRPoint q(vertexQ[0], vertexQ[1], vertexQ[2]);

        vertexIdx = meshIndices[idx + 2];
        odr::Vec3D vertexR = meshVertices[vertexIdx];
        OGRPoint r(vertexR[0], vertexR[1], vertexR[2]);

        OGRTriangle triangle(p, q, r);
        tin.addGeometry(&triangle);
    }

    return tin;
}