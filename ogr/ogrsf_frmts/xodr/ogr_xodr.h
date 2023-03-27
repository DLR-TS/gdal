/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of OGR driver components for OpenDRIVE.
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
 

#pragma once

#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include <iostream>
#include <OpenDrive/include/libopendrive.h>
#include <OpenDrive/thirdparty/pugixml/pugixml.hpp>
#include <vector>

/*--------------------------------------------------------------------*/
/*---------------------  Layer declerations  -------------------------*/
/*--------------------------------------------------------------------*/
class OGRXODRLayer : public OGRLayer
{
    OGRFeatureDefn     *poFeatureDefn;
    VSILFILE               *fp;
    int                 nNextFID;
    OGRSpatialReference *poSRS;
    

public:
    OGRXODRLayer( const char *pszFilename, uint8_t layer);
~OGRXODRLayer();

    uint8_t             layerID;
    std::string         fileName;
    
   
    void                ResetReading();
    OGRFeature *        GetNextFeature();

    OGRFeatureDefn *    GetLayerDefn() { return poFeatureDefn; }

    int                 TestCapability( const char * ) { return FALSE; }
    
private:
    enum layer_value {
        header = 0
    };
    
    OGRFeature*         getLayer();
    OGRFeature*         getRefLine();
    OGRFeature*         getLanes();
   
       
};



/*--------------------------------------------------------------------*/
/*--------------------  Datasource declerations ----------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRDataSource : public GDALDataset
{
    CPLString          pszName;
    OGRXODRLayer       **papoLayers;
    int                 nLayers;


public:
                        OGRXODRDataSource();
                        ~OGRXODRDataSource();

    int                 Open(const char *pszFilename, int bUpdate);

    int                 GetLayerCount() { return nLayers; }
    OGRLayer            *GetLayer( int );

    int                 TestCapability( const char * ) { return FALSE; }
   
};

/*--------------------------------------------------------------------*/
/*--------------------    Driver declerations   ----------------------*/
/*--------------------------------------------------------------------*/

static GDALDataset* OGRXODRDriverOpen(GDALOpenInfo* poOpenInfo);
static int          OGRXODRDriverIdentify(GDALOpenInfo* poOpenInfo);
static GDALDataset* OGRXODRDriverCreate(const char* pszName, int nXSize, int nYSize,
                                    int nBands, GDALDataType eDT, char** papszOptions);
/*--------------------------------------------------------------------*/


