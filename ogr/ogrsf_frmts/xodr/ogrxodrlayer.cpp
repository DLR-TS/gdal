
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
using namespace odr;
using namespace pugi;
using namespace std;
/*--------------------------------------------------------------------*/

OGRXODRLayer::OGRXODRLayer( const char *pszFilename, uint8_t layer )
{

    fileName = pszFilename;
    nNextFID = 0;
    poFeatureDefn = new OGRFeatureDefn(CPLGetBasename(pszFilename));
    //SetDescription(poFeatureDefn->GetName());
    //poFeatureDefn->Reference();
    //poFeatureDefn->SetGeomType(wkbPoint);

    //OGRFieldDefn oFieldTemplate("Name", OFTString);

    //poFeatureDefn->AddFieldDefn(&oFieldTemplate);

    /*-------------------------------*/
    //add xodr 
    //odr::OpenDriveMap xodrMap(pszFilename);
 
    /*-------------------------------*/
    //georeferencing
    xml_document doc;
    xml_parse_result result = doc.load_file(pszFilename);
    xml_node opendrive = doc.child("OpenDRIVE");
    xml_node header = opendrive.child("header");
    /*-------*/
    xml_attribute revMajor = header.attribute("revMajor");
    xml_attribute revMinor = header.attribute("revMinor");
    xml_attribute name = header.attribute("name");
    xml_attribute date = header.attribute("date");
    xml_attribute north = header.attribute("north");
    xml_attribute south = header.attribute("south");
    xml_attribute east = header.attribute("east");
    xml_attribute west = header.attribute("west");
    xml_attribute vendor = header.attribute("vendor");
    /*-------*/
    
    /*-------------------------------*/
    layerID = layer;
    if(layerID == 1){
      poFeatureDefn->SetGeomType(wkbPoint);
    }
    poFeatureDefn->Reference();
    ResetReading();
    poSRS = NULL;

    
    string georeference = header.child("geoReference").child_value();
    if(!georeference.empty()){
      poSRS = new OGRSpatialReference();
      poSRS -> importFromProj4(georeference.c_str());
      poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);
    }
    getLayer();
    /*-------------------------------*/
    fp = VSIFOpenL(pszFilename, "r");
    if( fp == NULL )
        return;
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
  if(layerID == 1){ //reference line
    return getRefLine();

  }else if(layerID == 2){ //lane
    return getLanes();
  }
   
  
}


void OGRXODRLayer::ResetReading()
{
    VSIFSeekL(fp, 0, SEEK_SET);
    nNextFID = 0;
}
/*--------------------------------------------------------------------*/
/*---------------------------     Layer    ----------------------------*/
/*--------------------------------------------------------------------*/



OGRFeature *OGRXODRLayer::getLayer(){

  if(layerID == 1){ //reference line
    OGRFieldDefn oFieldName("Name", OFTString);
    OGRFieldDefn oFieldID("ID", OFTString);
    OGRFieldDefn oFieldJunction("Junction", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldName);
    poFeatureDefn->AddFieldDefn(&oFieldID);
    poFeatureDefn->AddFieldDefn(&oFieldJunction);
  }else if(layerID == 2){ //lane
    OGRFieldDefn oFieldID("ID", OFTString);
    OGRFieldDefn oFieldType("Type", OFTString);
    OGRFieldDefn oFieldLevel("Level", OFTString);
    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldID);
    poFeatureDefn->AddFieldDefn(&oFieldType);
    poFeatureDefn->AddFieldDefn(&oFieldLevel);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);
  }
   
 
  return NULL;
}


/*--------------------------------------------------------------------*/
/*---------------------------    RefLine  ----------------------------*/
/*--------------------------------------------------------------------*/

OGRFeature *OGRXODRLayer::getRefLine(){
  odr::OpenDriveMap xodr(fileName);
  for(odr::Road road : xodr.get_roads()){
    odr::Vec3D test_point = road.ref_line.get_xyz(0.0);
    OGRFeature *feature = new OGRFeature(poFeatureDefn);
    OGRPoint *pFeature = new OGRPoint(test_point[0], test_point[1]);
    feature->SetGeometryDirectly(pFeature);
  }
}


/*--------------------------------------------------------------------*/
/*---------------------------     Lane    ----------------------------*/
/*--------------------------------------------------------------------*/

OGRFeature *OGRXODRLayer::getLanes(){
  odr::OpenDriveMap xodr(fileName);

  for(odr::Road road : xodr.get_roads()){
    for(odr::LaneSection lanesection : road.get_lanesections()){
      const double s_start = lanesection.s0;
      const double s_end = road.get_lanesection_end(lanesection);
    }
  }
}

