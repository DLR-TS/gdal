
/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRLayer.
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
#include "ogr_api.h"
#include "ogr_geometry.h"


#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include "cpl_error.h"

/*--------------------------------------------------------------------*/
OGRXODRLayer::OGRXODRLayer( const char *pszFilename, uint8_t layer_id)
{
    nNextFID = 0;

    poFeatureDefn = new OGRFeatureDefn(CPLGetBasename(pszFilename));
    SetDescription(poFeatureDefn->GetName());
    poFeatureDefn->Reference();
    poFeatureDefn->SetGeomType(wkbPoint);

    OGRFieldDefn oFieldTemplate("Name", OFTString);

    poFeatureDefn->AddFieldDefn(&oFieldTemplate);

    fp = VSIFOpenL(pszFilename, "r");
    if( fp == NULL )
        return;
    
    setLayer();
    
}

/*The layer constructor is responsible for initialization. 
The most important initialization is setting up the OGRFeatureDefn for the layer. 
This defines the list of fields and their types, the geometry type and the coordinate system for the layer.
In the SPF format the set of fields is fixed - a single string field and we have no coordinate system info to set.*/



OGRXODRLayer::~OGRXODRLayer()
{
    poFeatureDefn->Release();
    if( fp != NULL )
      VSIFCloseL(fp);
  
}
/*--------------------------------------------------------------------*/
/*--------------------      Layer iteration     ----------------------*/
/*--------------------------------------------------------------------*/
OGRFeature *OGRXODRLayer::GetNextFeature()
{
  switch(layer_id){
    case 0:
      return getHeader();
      break;
    default:
      break;
  }
}

void OGRXODRLayer::setLayer() {
  switch(layer_id){
    case 0:{
      OGRFieldDefn oFieldName("Name", OFTString);
      OGRFieldDefn oFieldID("ID", OFTString);
      break;
    }
    default:
      break;
    
  }
}


void OGRXODRLayer::ResetReading()
{
    
}

/*--------------------------------------------------------------------*/
/*--------------------          Header          ----------------------*/
/*--------------------------------------------------------------------*/
//TODO save as meta data 

OGRFeature *OGRXODRLayer::getHeader(){

  return NULL;
}

/*--------------------------------------------------------------------*/
/*---------------------------     Road    ----------------------------*/
/*--------------------------------------------------------------------*/


OGRFeature *OGRXODRLayer::getRoad(){

  return NULL;
}


