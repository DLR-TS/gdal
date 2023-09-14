/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRLayer.
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

OGRXODRLayer::OGRXODRLayer(VSILFILE *filePtr,
                           RoadElements xodrRoadElements, std::string proj4Defn,
                           bool dissolveTriangulatedSurface): 
    file(filePtr),
    roadElements(xodrRoadElements), // TODO For lower memory consumption maybe better pass by reference?
    dissolveSurface(dissolveTriangulatedSurface)
{
    spatialRef.importFromProj4(proj4Defn.c_str());
    ResetReading();
}

OGRXODRLayer::~OGRXODRLayer()
{
    if (featureDefn)
    {
        featureDefn->Release();
    }
}

void OGRXODRLayer::ResetReading()
{
    VSIFSeekL(file, 0, SEEK_SET);
    nNextFID = 0;
    resetRoadElementIterators();
}

void OGRXODRLayer::resetRoadElementIterators()
{
    roadIter = roadElements.roads.begin();

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