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
    papoLayers = nullptr;
    nLayers = 0;
}

OGRXODRDataSource::~OGRXODRDataSource()
{
     for( int i = 0; i < nLayers; i++ )
     {
        delete papoLayers[i];
        papoLayers[i] = nullptr;
     }
    CPLFree(papoLayers);
}

int  OGRXODRDataSource::Open( const char *fileName, int bUpdate )
{
    VSILFILE *filePointer = nullptr;

    bool updatable = CPL_TO_BOOL(bUpdate);
    if(updatable)
    {
        CPLError(CE_Failure, CPLE_OpenFailed, "Write or update access not supported by XODR driver.");
        return CE_Failure;
    } else 
    {
        filePointer = VSIFOpenExL(fileName, "r", true);
    }
    
    if( filePointer == nullptr )
    {
        //TODO is this ever called on an opening error? An incorrect file name or path is caught earlier already.
        CPLError(CE_Failure, CPLE_OpenFailed, "Failed to load OpenDRIVE file %s.", fileName);
        return CE_Failure;
    }
   
    // Create a corresponding layer.
    nLayers = 6;
    odr::OpenDriveMap xodr(fileName);
    xml_document doc;
    xml_parse_result result = doc.load_file(fileName);
    xml_node opendrive = doc.child("OpenDRIVE");
    xml_node header = opendrive.child("header");
    std::string refSystem = header.child("geoReference").child_value();
    roads = xodr.get_roads();
   
    papoLayers = (OGRXODRLayer **)CPLRealloc(papoLayers, sizeof(OGRXODRLayer *) * nLayers);
    //papoLayers = static_cast<OGRXODRLayer **>(CPLMalloc(sizeof(void *)));
    string layerName = "ReferenceLine";
    papoLayers[0] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    layerName = "LaneBorder";
    papoLayers[1] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    layerName = "RoadMark";
    papoLayers[2] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    layerName = "RoadObject";
    papoLayers[3] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    layerName = "RoadSignal";
    papoLayers[4] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    layerName = "Lane";
    papoLayers[5] = new OGRXODRLayer(fileName, filePointer, layerName.c_str(), layerName, roads, refSystem);
    
    return TRUE;
}

OGRLayer *OGRXODRDataSource::GetLayer( int iLayer )
{
    if( iLayer < 0 || iLayer >= nLayers )
        return NULL;
    
    return papoLayers[iLayer];
}

int OGRXODRDataSource::TestCapability( CPL_UNUSED const char * pszCap )

{
    return FALSE;
}

