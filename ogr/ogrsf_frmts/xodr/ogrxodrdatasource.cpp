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


CPL_CVSID("$Id$")
/*--------------------------------------------------------------------*/

OGRXODRDataSource::OGRXODRDataSource()
{
    papoLayers = NULL;
    nLayers = 0;
}

OGRXODRDataSource::~OGRXODRDataSource()
{
    for( int i = 0; i < nLayers; i++ )
        delete papoLayers[i];
    CPLFree(papoLayers);

    
}

/*--------------------------------------------------------------------*/
int  OGRXODRDataSource::Open( const char *pszFilename, int bUpdate )
{
    if( bUpdate )
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                "Update access not supported by the xodr driver.");
        return FALSE;
    }

    VSILFILE* fp = VSIFOpenL(pszFilename, "r");
    if (fp == nullptr)
        return FALSE;
    
    // Create a corresponding layer.
    nLayers = 3;
    papoLayers = (OGRXODRLayer **) CPLRealloc(papoLayers, nLayers * sizeof(OGRXODRLayer*));


    papoLayers[0] = new OGRXODRLayer(pszFilename,0);
    papoLayers[1] = new OGRXODRLayer(pszFilename,1);
    papoLayers[2] = new OGRXODRLayer(pszFilename,2);
    //pszName = CPLStrdup(pszFilename);

    return TRUE;
}


/*--------------------------------------------------------------------*/

OGRLayer *OGRXODRDataSource::GetLayer( int iLayer )
{
    if( iLayer < 0 || iLayer >= nLayers )
        return NULL;
    
    return papoLayers[iLayer];
}

/*--------------------------------------------------------------------*/

