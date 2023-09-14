/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRDataSource.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2023 German Aerospace Center (DLR), Institute of Transportation Systems
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

#include "ogr_xodr.h"
using namespace odr;
using namespace pugi;
using namespace std;

CPL_CVSID("$Id$")

OGRXODRDataSource::OGRXODRDataSource()
{
    layers = nullptr;
    nLayers = 0;
}

OGRXODRDataSource::~OGRXODRDataSource()
{
    for (int i = 0; i < nLayers; i++)
    {
        delete layers[i];
        layers[i] = nullptr;
    }
    CPLFree(layers);
}

int OGRXODRDataSource::Open(const char *fileName, int bUpdate)
{
    VSILFILE *file = nullptr;

    bool updatable = CPL_TO_BOOL(bUpdate);
    if (updatable)
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                 "Write or update access not supported by XODR driver.");
        return CE_Failure;
    }
    else
    {
        file = VSIFOpenExL(fileName, "r", true);
    }

    if (file == nullptr)
    {
        //TODO is this ever called on an opening error? An incorrect file name or path is caught earlier already.
        CPLError(CE_Failure, CPLE_OpenFailed,
                 "Failed to load OpenDRIVE file %s.", fileName);
        return CE_Failure;
    }

    nLayers = 5;
    odr::OpenDriveMap xodr(fileName, false);
    std::string proj4Defn = xodr.proj4;
    std::vector<odr::Road> roads = xodr.get_roads();
    RoadElements roadElements = createRoadElements(roads);

    layers = (OGRXODRLayer **)CPLRealloc(layers, sizeof(OGRXODRLayer *) * nLayers); //TODO update this to our new different classes
    layers[0] = new OGRXODRLayerReferenceLine(roadElements, proj4Defn);
    layers[1] = new OGRXODRLayerLaneBorder(roadElements, proj4Defn);
    layers[2] = new OGRXODRLayerRoadMark(roadElements, proj4Defn, false);
    layers[3] = new OGRXODRLayerRoadObject(roadElements, proj4Defn, false);
    layers[4] = new OGRXODRLayerLane(roadElements, proj4Defn, false);

    return TRUE;
}

OGRLayer *OGRXODRDataSource::GetLayer(int iLayer)
{
    if (iLayer < 0 || iLayer >= nLayers)
        return NULL;

    return layers[iLayer];
}

int OGRXODRDataSource::TestCapability(CPL_UNUSED const char *capability)
{
    if (EQUAL(capability, ODsCCreateLayer))
        return TRUE;
    else if (EQUAL(capability, ODsCDeleteLayer))
        return FALSE;
    else if (EQUAL(capability, ODsCZGeometries))
        return TRUE;
    return FALSE;
}

RoadElements OGRXODRDataSource::createRoadElements(const std::vector<odr::Road> roads,
                                                   const double eps)
{
    RoadElements elements;

    for (odr::Road road : roads)
    {
        elements.roads.push_back(road);

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

            odr::Mesh3D roadObjectMesh = road.get_road_object_mesh(roadObject, eps);
            elements.roadObjectMeshes.push_back(roadObjectMesh);
        }
    }
    return elements;
}
