/******************************************************************************
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRDataSource.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2024 German Aerospace Center (DLR), Institute of Transportation Systems
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_xodr.h"
using namespace odr;
using namespace pugi;
using namespace std;


OGRXODRDataSource::OGRXODRDataSource()
{

}

OGRXODRDataSource::~OGRXODRDataSource()
{

}

bool OGRXODRDataSource::Open(const char *pszFilename, CSLConstList openOptions)
{
    VSILFILE *file = nullptr;

    file = VSIFOpenL(pszFilename, "r");

    if (file == nullptr)
        return FALSE;

    const char *openOptionValue =
        CSLFetchNameValueDef(openOptions, "EPSILON", "1.0");
    eps = CPLAtof(openOptionValue);
    openOptionValue = CSLFetchNameValueDef(openOptions, "DISSOLVE_TIN", "NO");
    m_bDissolveTIN = CPLTestBool(openOptionValue);

    odr::OpenDriveMap xodr(pszFilename, false);
    std::string proj4Defn = xodr.proj4;
    std::vector<odr::Road> roads = xodr.get_roads();
    RoadElements roadElements = createRoadElements(roads);

    std::unique_ptr<OGRXODRLayer> refLine(new OGRXODRLayerReferenceLine(roadElements, proj4Defn));
    std::unique_ptr<OGRXODRLayer> laneBorder(new OGRXODRLayerLaneBorder(roadElements, proj4Defn));
    std::unique_ptr<OGRXODRLayer> roadMark(new OGRXODRLayerRoadMark(roadElements, proj4Defn, m_bDissolveTIN));
    std::unique_ptr<OGRXODRLayer> roadObject(new OGRXODRLayerRoadObject(roadElements, proj4Defn));
    std::unique_ptr<OGRXODRLayer> lane(new OGRXODRLayerLane(roadElements, proj4Defn, m_bDissolveTIN));
    std::unique_ptr<OGRXODRLayer> roadSignal(new OGRXODRLayerRoadSignal(roadElements, proj4Defn, m_bDissolveTIN));
    
    m_apoLayers.push_back(std::move(refLine));
    m_apoLayers.push_back(std::move(laneBorder));
    m_apoLayers.push_back(std::move(roadMark));
    m_apoLayers.push_back(std::move(roadObject));
    m_apoLayers.push_back(std::move(lane));
    m_apoLayers.push_back(std::move(roadSignal));
    
    return TRUE;
}

OGRLayer *OGRXODRDataSource::GetLayer(int iLayer)
{
    if (iLayer < 0 || (size_t)iLayer >= m_apoLayers.size())
        return NULL;

    return m_apoLayers[iLayer].get();
}

int OGRXODRDataSource::TestCapability(CPL_UNUSED const char *capability)
{
    if (EQUAL(capability, ODsCZGeometries))
        return TRUE;
    return FALSE;
}

RoadElements
OGRXODRDataSource::createRoadElements(const std::vector<odr::Road>& roads)
{
    RoadElements elements;

    for (odr::Road road : roads)
    {
        elements.roads.insert({road.id, road});

        odr::Line3D referenceLine =
            road.ref_line.get_line(0.0, road.length, eps);
        elements.referenceLines.push_back(referenceLine);

        for (odr::LaneSection laneSection : road.get_lanesections())
        {
            elements.laneSections.push_back(laneSection);

            for (odr::Lane lane : laneSection.get_lanes())
            {
                elements.laneRoadIDs.push_back(road.id);

                elements.lanes.push_back(lane);

                odr::Mesh3D laneMesh = road.get_lane_mesh(lane, eps);
                elements.laneMeshes.push_back(laneMesh);

                odr::Line3D laneLineOuter =
                    road.get_lane_border_line(lane, eps, true);
                elements.laneLinesOuter.push_back(laneLineOuter);

                odr::Line3D laneLineInner =
                    road.get_lane_border_line(lane, eps, false);
                elements.laneLinesInner.push_back(laneLineInner);

                for (odr::RoadMark roadMark : lane.get_roadmarks(
                         laneSection.s0, road.get_lanesection_end(laneSection)))
                {
                    elements.roadMarks.push_back(roadMark);

                    odr::Mesh3D roadMarkMesh =
                        road.get_roadmark_mesh(lane, roadMark, eps);
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

        for (odr::RoadSignal roadSignal : road.get_road_signals())
        {
            elements.roadSignals.push_back(roadSignal);

            odr::Mesh3D roadSignalMesh = road.get_road_signal_mesh(roadSignal);
            elements.roadSignalMeshes.push_back(roadSignalMesh);
        }
    }
    return elements;
}
