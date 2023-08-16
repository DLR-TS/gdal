
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
using namespace pugi;
using namespace std;

OGRXODRLayer::OGRXODRLayer(VSILFILE *filePtr, std::string name,
                           std::vector<odr::Road> xodrRoads, std::string proj4Defn): 
    file(filePtr), 
    layerName(name), 
    roads(xodrRoads), 
    nNextFID(0), 
    spatialRef(NULL)
{
    initXodrElements();

    this->featureDefn = new OGRFeatureDefn(layerName.c_str());
    SetDescription(featureDefn->GetName());

    if (layerName == "ReferenceLine" || layerName == "LaneBorder" ||
        layerName == "RoadMark" || layerName == "RoadObject")
    {

        featureDefn->SetGeomType(wkbMultiLineString);
    }
    else if (layerName == "Lane")
    {
        featureDefn->SetGeomType(wkbMultiPolygon);
    }

    featureDefn->Reference();
    ResetReading();
    spatialRef = new OGRSpatialReference();
    spatialRef->importFromProj4(proj4Defn.c_str());
    featureDefn->GetGeomFieldDefn(0)->SetSpatialRef(spatialRef);

    defineFeatureClass();
}

OGRXODRLayer::~OGRXODRLayer()
{
    if (featureDefn != NULL)
    {
        featureDefn->Release();
    }

    if (spatialRef)
        spatialRef->Release();
}
/*--------------------------------------------------------------------*/
/*--------------------      Layer iteration     ----------------------*/
/*--------------------------------------------------------------------*/

OGRFeature *OGRXODRLayer::GetNextFeature()
{
    if (layerName == "ReferenceLine")
    {
        if (roadIter != roads.end())
        {
            std::unique_ptr<OGRFeature> feature(new OGRFeature(featureDefn));
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
            if ((m_poFilterGeom == nullptr ||
                 FilterGeometry(feature->GetGeometryRef())) &&
                (m_poAttrQuery == nullptr ||
                 m_poAttrQuery->Evaluate(feature.get())))
            {
                return feature.release();
            }
        }
    }
    /*---------------------------     Lane    ----------------------------*/
    if ((layerName == "LaneBorder") && (laneMeshesIter != laneMeshes.end()))
    {
        OGRLineString lineStringEven;
        OGRLineString lineStringOdd;
        std::unique_ptr<OGRFeature> feature(new OGRFeature(featureDefn));
        std::unique_ptr<OGRMultiLineString> multiLineString(new OGRMultiLineString());

        odr::Mesh3D laneMesh = *laneMeshesIter;
        odr::Lane lane = *laneIter;
        std::string laneRoadID = *lanesRoadIDsIter;

        std::vector<odr::Vec3D> laneVertices = laneMesh.vertices;
        for (std::size_t iVertex = 0; iVertex < laneVertices.size(); iVertex++)
        {
            odr::Vec3D laneVertex = laneVertices[iVertex];
            if (iVertex % 2 != 0)
            {
                lineStringEven.addPoint(laneVertex[0], laneVertex[1]);
            }
            else
            {
                lineStringOdd.addPoint(laneVertex[0], laneVertex[1]);
            }
        }

        multiLineString->addGeometry(&lineStringEven);
        multiLineString->addGeometry(&lineStringOdd);
        std::unique_ptr<OGRGeometry> geometry(multiLineString->MakeValid());
        feature->SetGeometry(geometry.get());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"), laneRoadID.c_str());
        feature->SetField(featureDefn->GetFieldIndex("ID"), lane.id);
        feature->SetField(featureDefn->GetFieldIndex("Type"), lane.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Predecessor"), lane.predecessor);
        feature->SetField(featureDefn->GetFieldIndex("Successor"), lane.successor);
        feature->SetFID(nNextFID++);

        lanesRoadIDsIter++;
        laneIter++;
        laneMeshesIter++;

        if ((m_poFilterGeom == nullptr ||
             FilterGeometry(feature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr ||
             m_poAttrQuery->Evaluate(feature.get())))
        {
            return feature.release();
        }
    }
    /*---------------------------   RoadMark  ----------------------------*/
    if ((layerName == "RoadMark") && (roadMarkMeshesIter != roadMarkMeshes.end()))
    {
        OGRLineString lineStringEven;
        OGRLineString lineStringOdd;

        std::unique_ptr<OGRFeature> feature(new OGRFeature(featureDefn));
        std::unique_ptr<OGRMultiLineString> multiLineString(new OGRMultiLineString());

        odr::Mesh3D roadMarkMesh = *roadMarkMeshesIter;
        odr::RoadMark roadMark = *roadMarkIter;

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
        multiLineString->addGeometry(&lineStringEven);
        multiLineString->addGeometry(&lineStringOdd);

        std::unique_ptr<OGRGeometry> geometry(multiLineString->MakeValid());
        feature->SetGeometry(geometry.get());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"), roadMark.road_id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("LaneID"), roadMark.lane_id);
        feature->SetField(featureDefn->GetFieldIndex("Type"), roadMark.type.c_str());
        feature->SetFID(nNextFID++);

        roadMarkIter++;
        roadMarkMeshesIter++;

        if ((m_poFilterGeom == nullptr ||
             FilterGeometry(feature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr ||
             m_poAttrQuery->Evaluate(feature.get())))
        {
            return feature.release();
        }
    }
    /*--------------------------- RoadObjects ----------------------------*/
    if ((layerName == "RoadObject") && (roadObjectMeshesIter != roadObjectMeshes.end()))
    {
        OGRLineString lineString;

        std::unique_ptr<OGRFeature> feature(new OGRFeature(featureDefn));
        std::unique_ptr<OGRMultiLineString> multiLineString(new OGRMultiLineString());
        
        const odr::Mesh3D &roadObjectMesh = *roadObjectMeshesIter;
        odr::RoadObject roadObject = *roadObjectIter;
        const std::vector<odr::Vec3D> roadObjectVertices = roadObjectMesh.vertices;

        for (std::size_t iVertex = 0; iVertex < roadObjectVertices.size(); iVertex++)
        {
            const odr::Vec3D &roadObjectVertex = roadObjectVertices[iVertex];
            lineString.addPoint(roadObjectVertex[0], roadObjectVertex[1]);
        }
        multiLineString->addGeometry(&lineString);
        std::unique_ptr<OGRGeometry> geometry(multiLineString->MakeValid());
        feature->SetGeometry(geometry.get());
        feature->SetField(featureDefn->GetFieldIndex("ObjectID"), roadObject.id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"), roadObject.road_id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Type"), roadObject.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Name"), roadObject.name.c_str());
        feature->SetFID(nNextFID++);

        roadObjectIter++;
        roadObjectMeshesIter++;

        if ((m_poFilterGeom == nullptr ||
             FilterGeometry(feature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr ||
             m_poAttrQuery->Evaluate(feature.get())))
        {
            return feature.release();
        }
    }
    /*--------------------------- Lanes ----------------------------*/
    if ((layerName == "Lane") && (laneMeshesIter != laneMeshes.end()))
    {
        OGRLineString lineStringEven;
        OGRLineString lineStringOdd;

        std::unique_ptr<OGRFeature> feature(new OGRFeature(featureDefn));
        std::unique_ptr<OGRMultiPolygon> multiPolygon(new OGRMultiPolygon());

        odr::Mesh3D laneMesh = *laneMeshesIter;
        odr::Lane lane = *laneIter;
        std::string laneRoadID = *lanesRoadIDsIter;

        std::vector<odr::Vec3D> laneVertices = laneMesh.vertices;
        for (std::size_t iVertex = 0; iVertex < laneVertices.size(); iVertex++)
        {
            odr::Vec3D laneVertex = laneVertices[iVertex];
            if (iVertex % 2 != 0)
            {
                lineStringEven.addPoint(laneVertex[0], laneVertex[1]);
            }
            else
            {
                lineStringOdd.addPoint(laneVertex[0], laneVertex[1]);
            }
        }

        OGRLinearRing ring;
        for (int i = 0; i < lineStringEven.getNumPoints(); ++i)
        {
            OGRPoint point;
            lineStringEven.getPoint(i, &point);
            ring.addPoint(point.getX(), point.getY());
        }
        for (int i = lineStringOdd.getNumPoints() - 1; i >= 0; --i)
        {
            OGRPoint point;
            lineStringOdd.getPoint(i, &point);
            ring.addPoint(point.getX(), point.getY());
        }
        OGRPoint startPoint;
        lineStringEven.getPoint(0, &startPoint);
        ring.addPoint(startPoint.getX(), startPoint.getY());

        std::unique_ptr<OGRPolygon> polygon(new OGRPolygon());
        polygon->addRing(&ring);

        std::unique_ptr<OGRGeometry> geometry(polygon->MakeValid());
        feature->SetGeometry(geometry.get());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"), laneRoadID.c_str());
        feature->SetField(featureDefn->GetFieldIndex("LaneID"), lane.id);
        feature->SetField(featureDefn->GetFieldIndex("Type"), lane.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Predecessor"), lane.predecessor);
        feature->SetField(featureDefn->GetFieldIndex("Successor"), lane.successor);
        feature->SetFID(nNextFID++);

        lanesRoadIDsIter++;
        laneIter++;
        laneMeshesIter++;

        if ((m_poFilterGeom == nullptr ||
             FilterGeometry(feature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr ||
             m_poAttrQuery->Evaluate(feature.get())))
        {
            return feature.release();
        }
    }

    return nullptr;
}

/*--------------------------------------------------------------------*/
/*--------------------       Reset Reading      ----------------------*/
/*--------------------------------------------------------------------*/
void OGRXODRLayer::ResetReading()
{
    VSIFSeekL(file, 0, SEEK_SET);
    nNextFID = 0;
}
/*--------------------------------------------------------------------*/
/*---------------------------     Layer    ----------------------------*/
/*--------------------------------------------------------------------*/

void OGRXODRLayer::defineFeatureClass()
{
    if (layerName == "ReferenceLine")
    {
        OGRFieldDefn oFieldID("ID", OFTString);
        featureDefn->AddFieldDefn(&oFieldID);

        OGRFieldDefn oFieldLen("Length", OFTReal);
        featureDefn->AddFieldDefn(&oFieldLen);

        OGRFieldDefn oFieldJunction("Junction", OFTString);
        featureDefn->AddFieldDefn(&oFieldJunction);
    }
    else if (layerName == "LaneBorder")
    {
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
    else if (layerName == "RoadMark")
    {
        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
        featureDefn->AddFieldDefn(&oFieldLaneID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);

        OGRFieldDefn oFieldName("Name", OFTString);
        featureDefn->AddFieldDefn(&oFieldName);
    }
    else if (layerName == "RoadObject")
    {
        OGRFieldDefn oFieldObjectID("ObjectID", OFTString);
        featureDefn->AddFieldDefn(&oFieldObjectID);

        OGRFieldDefn oFieldRoadID("RoadID", OFTString);
        featureDefn->AddFieldDefn(&oFieldRoadID);

        OGRFieldDefn oFieldType("Type", OFTString);
        featureDefn->AddFieldDefn(&oFieldType);

        OGRFieldDefn oFieldObjectName("Name", OFTString);
        featureDefn->AddFieldDefn(&oFieldObjectName);
    }
    else if (layerName == "Lane")
    {
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
/*--------------------------------------------------------------------*/
/*------------------------ Initialization   --------------------------*/
/*--------------------------------------------------------------------*/
void OGRXODRLayer::initXodrElements()
{
    RoadElements roadElements = getRoadElements();

    roadIter = roads.begin();

    laneSections = roadElements.laneSections;
    laneSectionIter = laneSections.begin();

    lanes = roadElements.lanes;
    laneIter = lanes.begin();

    lanesRoadIDs = roadElements.laneRoadID;
    lanesRoadIDsIter = lanesRoadIDs.begin();

    laneMeshes = roadElements.laneMeshes;
    laneMeshesIter = laneMeshes.begin();

    roadMarks = roadElements.roadMarks;
    roadMarkIter = roadMarks.begin();

    roadMarkMeshes = roadElements.roadMarkMeshes;
    roadMarkMeshesIter = roadMarkMeshes.begin();

    roadObjects = roadElements.roadObjects;
    roadObjectIter = roadObjects.begin();

    roadObjectMeshes = roadElements.roadObjectMeshes;
    roadObjectMeshesIter = roadObjectMeshes.begin();
}

/*--------------------------------------------------------------------*/
/*------------------------ getRoadElements   --------------------------*/
/*--------------------------------------------------------------------*/
OGRXODRLayer::RoadElements OGRXODRLayer::getRoadElements()
{
    RoadElements roadElements;

    const double eps = 0.5;
    //for (int road = 0; road < Roads.size(); road++) {
    for (odr::Road road : roads)
    {
        for (odr::LaneSection laneSection : road.get_lanesections())
        {
            roadElements.laneSections.push_back(laneSection);

            for (odr::Lane lane : laneSection.get_lanes())
            {
                roadElements.laneRoadID.push_back(road.id);

                roadElements.lanes.push_back(lane);

                odr::Mesh3D laneMesh = road.get_lane_mesh(lane, eps);
                roadElements.laneMeshes.push_back(laneMesh);

                for (odr::RoadMark roadMark : lane.get_roadmarks(
                         laneSection.s0, road.get_lanesection_end(laneSection)))
                {

                    roadElements.roadMarks.push_back(roadMark);

                    odr::Mesh3D roadMarkMesh =
                        road.get_roadmark_mesh(lane, roadMark, eps);
                    roadElements.roadMarkMeshes.push_back(roadMarkMesh);
                }
            }
        }

        for (odr::RoadObject roadObject : road.get_road_objects())
        {
            roadElements.roadObjects.push_back(roadObject);

            odr::Mesh3D roadObjectMesh =
                road.get_road_object_mesh(roadObject, eps);
            roadElements.roadObjectMeshes.push_back(roadObjectMesh);
        }
    }
    return roadElements;
}