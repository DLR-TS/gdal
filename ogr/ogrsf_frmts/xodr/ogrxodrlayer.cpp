
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

OGRXODRLayer::OGRXODRLayer( const char *pszFilename, VSILFILE  *fp, const char* pszLayerName, std::string layer ):
//poFeatureDefn(new OGRFeatureDefn(CPLGetBasename(pszLayerName))),
fpXODR(fp),
pszFilename(CPLStrdup(pszFilename)),
nNextFID(0)
{

  layerName = layer;
 
  poFeatureDefn = new OGRFeatureDefn(CPLGetBasename(pszLayerName));
  SetDescription(poFeatureDefn->GetName());
  if(layerName == "refLine" || layerName == "Lanes" ||  layerName == "RoadMark" )  {
   
    poFeatureDefn->SetGeomType(wkbMultiLineString);  
  }
  poFeatureDefn->Reference();
  ResetReading();
  poSRS = NULL; //if this line is in if statement -segmentation fault error return on testing 
 
  /* Georeferencing */
  /*-------------------------------*/
  xml_document doc;
  xml_parse_result result = doc.load_file(pszFilename);
  xml_node opendrive = doc.child("OpenDRIVE");
  xml_node header = opendrive.child("header");
  string georeference = header.child("geoReference").child_value();
  if(!georeference.empty()){
    poSRS = new OGRSpatialReference();
    poSRS -> importFromProj4(georeference.c_str());
    poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);
  /*-------------------------------*/
  }
  
  

  getLayer();
    
    
}
/*The layer constructor is responsible for initialization. 
The most important initialization is setting up the OGRFeatureDefn for the layer. 
This defines the list of fields and their types, the geometry type and the coordinate system for the layer.
In the SPF format the set of fields is fixed - a single string field and we have no coordinate system info to set.*/

OGRXODRLayer::~OGRXODRLayer()
{
    if (poFeatureDefn != NULL)
	{
		poFeatureDefn->Release();
	}

    if (poSRS)
        poSRS->Release();
  
}
/*--------------------------------------------------------------------*/
/*--------------------      Layer iteration     ----------------------*/
/*--------------------------------------------------------------------*/
OGRFeature *OGRXODRLayer::GetNextFeature()
{
  if(layerName == "refLine"){ //reference line
    return getRefLine();  
  }else if( layerName == "Lanes"){
    return getLanes();
  }else if( layerName == "RoadMark"){
    return getRoadMark();
  }
  
}

void OGRXODRLayer::ResetReading()
{
    VSIFSeekL(fpXODR, 0, SEEK_SET);
    nNextFID = 0;
}
/*--------------------------------------------------------------------*/
/*---------------------------     Layer    ----------------------------*/
/*--------------------------------------------------------------------*/

//string OGRXODRLayer::getReferenceSystem(){
//    /* Georeferencing */
//    /*-------------------------------*/
//    //xml_document doc;
//    //xml_parse_result result = doc.load_file(fileName);
//    //xml_node opendrive = doc.child("OpenDRIVE");
//    //xml_node header = opendrive.child("header");
//    //string georeference = header.child("geoReference").child_value();
//  return ;
//}


OGRFeature *OGRXODRLayer::getLayer(){

  if(layerName == "refLine"){ //reference line
    OGRFieldDefn oFieldName("Name", OFTString);
    OGRFieldDefn oFieldID("ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldName);
    poFeatureDefn->AddFieldDefn(&oFieldID);
  }else if(layerName == "Lanes"){ 
    OGRFieldDefn oFieldID("ID", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldID);
  } else if(layerName == "RoadMark"){ 
    OGRFieldDefn oFieldName("Name", OFTString);
    OGRFieldDefn oFieldID("ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldName);
    poFeatureDefn->AddFieldDefn(&oFieldID);
  } 
 
  return NULL;
}


/*--------------------------------------------------------------------*/
/*---------------------------    RefLine  ----------------------------*/
/*--------------------------------------------------------------------*/

OGRFeature *OGRXODRLayer::getRefLine(){
  odr::OpenDriveMap xodr(pszFilename);
  std::vector<odr::Road> Roads = xodr.get_roads();
  const double      eps = 0.9;

  OGRFeature* poFeature = new OGRFeature(poFeatureDefn);
  OGRMultiLineString *poLS = new OGRMultiLineString();
  for(int i = 0; i < Roads.size();i++ ){
    odr::Road Road = Roads[i];
    odr::RefLine RefLine = Road.ref_line;
    std::set<double> s_vals = RefLine.approximate_linear(eps, 0.0, Road.length);
    OGRLineString *ls = new OGRLineString();
    for (const double& s : s_vals)
    {
      odr::Vec3D RoadGeometry = RefLine.get_xyz(s);
      ls-> addPoint( new OGRPoint(RoadGeometry[0], RoadGeometry[1]));
    }
    poLS->addGeometry(ls);
    OGRGeometry *poGeometry = poLS->MakeValid();
    poFeature->SetGeometry(poGeometry);
    poFeature->SetField(poFeatureDefn->GetFieldIndex("Name"), Road.id.c_str());
    poFeature->SetField(poFeatureDefn->GetFieldIndex("ID"), Road.id.c_str());
    poFeature->SetFID(nNextFID++);
    delete poGeometry;
  }
  if(FilterGeometry(poFeature->GetGeometryRef())){
    return poFeature;
  }else{
    delete poFeature;
  }
  return NULL;
}


/*--------------------------------------------------------------------*/
/*---------------------------     Lane    ----------------------------*/
/*--------------------------------------------------------------------*/

OGRFeature *OGRXODRLayer::getLanes(){
  OGRFeature* poFeature = new OGRFeature(poFeatureDefn);
  OGRMultiLineString *poLS = new OGRMultiLineString();
  
  odr::OpenDriveMap xodr(pszFilename);
  std::vector<odr::Road> Roads = xodr.get_roads();
  
  // epsilon for approximation 
  const double      eps = 0.9;
  for(int road = 0; road < Roads.size(); road++ ){
    odr::Road Road = Roads[road];
    std::vector<odr::LaneSection> LaneSections = Road.get_lanesections();
    for(int lanesection = 0; lanesection < LaneSections.size() ; lanesection ++){
      odr::LaneSection LaneSection = LaneSections[lanesection];
      std::vector<odr::Lane> Lanes = LaneSection.get_lanes();
      
      for(int lane = 0; lane < Lanes.size(); lane ++){
        OGRLineString lineString_even;
        OGRLineString lineString_odd;
        odr::Lane Lane = Lanes[lane];
        odr::Mesh3D LaneMeshRight = Road.get_lane_mesh(Lane, eps);
        std::vector<odr::Vec3D> LaneGeometriesRight = LaneMeshRight.vertices;

        for(int lanegeometry = 0; lanegeometry < LaneGeometriesRight.size() ; lanegeometry ++){
          odr::Vec3D LaneGeometryRight = LaneGeometriesRight[lanegeometry];
          
          if(lanegeometry % 2 != 0){
            lineString_even.addPoint(LaneGeometryRight[0], LaneGeometryRight[1]);
          } else {
            lineString_odd.addPoint(LaneGeometryRight[0], LaneGeometryRight[1]);
          }

        }
        poLS->addGeometry(&lineString_even);
        poLS->addGeometry(&lineString_odd);

        OGRGeometry *poGeometry = poLS->MakeValid();

        poFeature->SetGeometry(poGeometry);
        poFeature->SetField(poFeatureDefn->GetFieldIndex("ID"), Lane.id);
        poFeature->SetFID(nNextFID++);
        delete poGeometry;
      }
    }
  }
  
  if(FilterGeometry(poFeature->GetGeometryRef())){
    return poFeature;
  }else{
    delete poFeature;
  }
  return NULL;
}

OGRFeature *OGRXODRLayer::getRoadMark(){
  OGRFeature* poFeature = new OGRFeature(poFeatureDefn);
  OGRMultiLineString *poLS = new OGRMultiLineString();
  
  odr::OpenDriveMap xodr(pszFilename);
  std::vector<odr::Road> Roads = xodr.get_roads();

  const double      eps = 0.9;
  for(int road = 0; road < Roads.size(); road++ ){
    odr::Road Road = Roads[road];
    std::vector<odr::LaneSection> LaneSections = Road.get_lanesections();
    for(int lanesection = 0; lanesection < LaneSections.size() ; lanesection ++){
      odr::LaneSection LaneSection = LaneSections[lanesection];
      std::vector<odr::Lane> Lanes = LaneSection.get_lanes();
      
      for(int lane = 0; lane < Lanes.size(); lane ++){
        OGRLineString lineString_even;
        OGRLineString lineString_odd;
        odr::Lane Lane = Lanes[lane];
        const std::vector<RoadMark> RoadMarks = Lane.get_roadmarks(LaneSection.s0, Road.get_lanesection_end(LaneSection));
        for(int roadmark = 0; roadmark < RoadMarks.size(); roadmark ++){
          odr::RoadMark RoadMark = RoadMarks[roadmark];
          odr::Mesh3D RoadMarkMesh = Road.get_roadmark_mesh(Lane, RoadMark, eps);
          std::vector<odr::Vec3D> RoadMarkGeometries = RoadMarkMesh.vertices;
          for(int roadmarkgeometry = 0; roadmarkgeometry < RoadMarkGeometries.size(); roadmarkgeometry++){
            odr::Vec3D RoadMarkGeometry = RoadMarkGeometries[roadmarkgeometry];
            if(roadmarkgeometry %2 ==0){
              lineString_even.addPoint(RoadMarkGeometry[0], RoadMarkGeometry[1]);
            }else{
              lineString_odd.addPoint(RoadMarkGeometry[0], RoadMarkGeometry[1]);
            }
          }
        }
        poLS->addGeometry(&lineString_even);
        poLS->addGeometry(&lineString_odd);
        OGRGeometry *poGeometry = poLS->MakeValid();

        poFeature->SetGeometry(poGeometry);
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Name"), Road.id.c_str());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("ID"), Road.id.c_str());
        poFeature->SetFID(nNextFID++);
        delete poGeometry;
      }
    }
  }
  
  if(FilterGeometry(poFeature->GetGeometryRef())){
    return poFeature;
  }else{
    delete poFeature;
  }
  return NULL;
}

