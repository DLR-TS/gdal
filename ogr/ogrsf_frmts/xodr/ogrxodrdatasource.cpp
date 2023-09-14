/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Created:  - 2017
 * Modified: February 2023
 * Purpose:  Implementation of OGRXODRDataSource.
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center (DLR)
 *           Oliver Böttcher, oliver.boettcher@dlr.de, German Aerospace Center (DLR)
 *			 Cristhian Eduardo Murcia Galeano, cristhianmurcia182@gmail.com
 *			 Ana Maria Orozco, ana.orozco.net@gmail.com
 *           
 ******************************************************************************
 * Copyright 2022 German Aerospace Center (DLR), Institute of Transportation Systems
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

    layers = (OGRXODRLayer **)CPLRealloc(layers, sizeof(OGRXODRLayer *) * nLayers);
    layers[0] = new OGRXODRLayer(file, XODRLayerType::ReferenceLine, roads, proj4Defn);
    layers[1] = new OGRXODRLayer(file, XODRLayerType::LaneBorder, roads, proj4Defn);
    layers[2] = new OGRXODRLayer(file, XODRLayerType::RoadMark, roads, proj4Defn, false);
    layers[3] = new OGRXODRLayer(file, XODRLayerType::RoadObject, roads, proj4Defn, false);
    layers[4] = new OGRXODRLayer(file, XODRLayerType::Lane, roads, proj4Defn, false);

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
