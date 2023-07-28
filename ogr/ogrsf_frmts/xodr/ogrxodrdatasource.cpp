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
    papoLayers = NULL;
    nLayers = 0;
   
}

OGRXODRDataSource::~OGRXODRDataSource()
{
    int i;
    for( int i = 0; i < nLayers; i++ )
        delete papoLayers[i];
        papoLayers[i] = nullptr;
    CPLFree(papoLayers); 

}

int  OGRXODRDataSource::Open( const char *pszFilename, int bUpdate )
{
    
    bUpdate = CPL_TO_BOOL(bUpdate);
    CPLString osFilename(pszFilename);
    const CPLString osBaseFilename = CPLGetFilename(pszFilename);

    VSILFILE *fp = nullptr;

    if( bUpdate )
        fp = VSIFOpenExL(pszFilename, "rb+", true);
    else
        fp = VSIFOpenExL(pszFilename, "rb", true);
    if( fp == nullptr )
    {
        CPLError(CE_Warning, CPLE_OpenFailed, "Failed to open %s."); //TODO %s is never substituted
        return false;
    }


    if( bUpdate )//TODO merge with upper if condition on the same variable
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                "Update access not supported by the xodr driver.");
        return FALSE;
    }
   
    // Create a corresponding layer.
    nLayers = 5;
    fileName = pszFilename;
    odr::OpenDriveMap xodr(fileName);
    xml_document doc;
    xml_parse_result result = doc.load_file(pszFilename);
    xml_node opendrive = doc.child("OpenDRIVE");
    xml_node header = opendrive.child("header");
    std::string RefSystem = header.child("geoReference").child_value();
   
    vector<Road> roads = xodr.get_roads();
   
    papoLayers = (OGRXODRLayer **)CPLRealloc(papoLayers, sizeof(OGRXODRLayer *) * nLayers);
    //papoLayers = static_cast<OGRXODRLayer **>(CPLMalloc(sizeof(void *)));
    string layername = "ReferenceLine";
    papoLayers[0] = new OGRXODRLayer(pszFilename, fp, layername.c_str(), layername, roads, RefSystem);
    layername = "Lanes";
    papoLayers[1] = new OGRXODRLayer(pszFilename, fp, layername.c_str(),  layername, roads, RefSystem);
    layername = "RoadMark";
    papoLayers[2] = new OGRXODRLayer(pszFilename, fp, layername.c_str(), layername, roads, RefSystem);
    layername = "RoadObject";
    papoLayers[3] = new OGRXODRLayer(pszFilename, fp, layername.c_str(), layername, roads, RefSystem);
    layername = "RoadSignal";
    papoLayers[4] = new OGRXODRLayer(pszFilename, fp, layername.c_str(), layername, roads, RefSystem);
    //layername = "drive";
    //papoLayers[5] = new OGRXODRLayer(pszFilename, fp, layername.c_str(), layername, roads, RefSystem);
    
    
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

