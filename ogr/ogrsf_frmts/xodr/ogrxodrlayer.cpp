
/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRLayer.
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center
 *(DLR) Oliver Böttcher, oliver.boettcher@dlr.de, German Aerospace Center (DLR)
 *			 Cristhian Eduardo Murcia Galeano,
 *cristhianmurcia182@gmail.com Ana Maria Orozco, ana.orozco.net@gmail.com
 *
 ******************************************************************************
 * Copyright 2022 German Aerospace Center (DLR), Institute of Transportation
 *Systems
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
using namespace pugi;
using namespace std;
/*--------------------------------------------------------------------*/

OGRXODRLayer::OGRXODRLayer(const char *pszFilename, VSILFILE *fp,
                           const char *pszLayerName, std::string layer,
                           std::vector<odr::Road> roads, std::string refSystem):
    fpXODR(fp), 
    pszFilename(CPLStrdup(pszFilename)), 
    Roads(roads),
    RoadIter(Roads.begin()),
    nNextFID(0),
    poSRS(NULL){

  layerName = layer;
  std::vector<odr::LaneSection> LaneSections_ = getFullLaneSections();
  LaneSections = LaneSections_;
  LaneSectionIter = LaneSections.begin();
  
  std::vector<odr::Lane> Lanes_ = getFullLanes();
  Lanes = Lanes_;
  LaneIter = Lanes.begin();
  
  std::vector<odr::Mesh3D> LaneMeshes_ = getFullLaneMeshes();
  LaneMeshes = LaneMeshes_;
  LaneMeshesIter = LaneMeshes.begin();
  
  std::vector<odr::RoadMark> RoadMark_ = getFullRoadMarks();
  RoadMarks = RoadMark_;
  RoadMarkIter = RoadMarks.begin();

  std::vector<odr::Mesh3D> RoadMarkMeshes_ = getFullRoadMarkMeshes();
  RoadMarkMeshes = RoadMarkMeshes_;
  RoadMarkMeshesIter = RoadMarkMeshes.begin();

  std::vector<odr::RoadObject> RoadObject_ = getFullRoadObjects();
  RoadObjects = RoadObject_;
  RoadObjectIter = RoadObjects.begin();

  std::vector<odr::Mesh3D> RoadObjectMeshes_ = getFullRoadObjectMeshes();
  RoadObjectMeshes = RoadObjectMeshes_;
  RoadObjectMeshesIter = RoadObjectMeshes.begin();

  poFeatureDefn = new OGRFeatureDefn(CPLGetBasename(pszLayerName));
  SetDescription(poFeatureDefn->GetName());
  
  if (layerName == "refLine" || layerName == "Lanes" ||
      layerName == "RoadMark" ||
      layerName == "RoadObject") {
    
    poFeatureDefn->SetGeomType(wkbMultiLineString);
  }
  
  
  poFeatureDefn->Reference();
  ResetReading();
  poSRS = new OGRSpatialReference();
  poSRS->importFromProj4(refSystem.c_str());
  poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);
  //poSRS = NULL; // if this line is in if statement -segmentation fault error
                // return on testing
  getLayer();

  /* Georeferencing */
  /*-------------------------------*/
 
  //if (!georeference.empty()) {
  //  poSRS = new OGRSpatialReference();
  //  poSRS->importFromProj4(georeference.c_str());
  //  poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);
  //  /*-------------------------------*/
  //}
  
}

OGRXODRLayer::~OGRXODRLayer() {
  if (poFeatureDefn != NULL) {
    poFeatureDefn->Release();
  }

  if (poSRS)
    poSRS->Release();
}
/*--------------------------------------------------------------------*/
/*--------------------      Layer iteration     ----------------------*/
/*--------------------------------------------------------------------*/

OGRFeature* OGRXODRLayer::GetNextFeature()
{
    if(layerName == "refLine"){
      if (RoadIter != Roads.end()) {
          std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
          std::unique_ptr<OGRLineString> poLS(new OGRLineString());
          

          const double eps = 0.9;
          odr::Road Road = *RoadIter;
          odr::RefLine refLine = Road.ref_line;
          std::set<double> s_vals = refLine.approximate_linear(eps, 0.0, Road.length);

          for (const double& s : s_vals) {
              odr::Vec3D roadGeometry = refLine.get_xyz(s);
              poLS->addPoint(roadGeometry[0], roadGeometry[1]);
          }

          std::unique_ptr<OGRGeometry> poGeometry(poLS->MakeValid());
          poFeature->SetGeometry(poGeometry.get());
          poFeature->SetField("ID", Road.id.c_str());
          poFeature->SetField("Length", Road.length);
          poFeature->SetField("Junction", Road.junction.c_str());
          poFeature->SetFID(nNextFID++);

          RoadIter++;
          if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
              (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
              return poFeature.release();
          }
      }
    }
    /*---------------------------     Lane    ----------------------------*/
    if( layerName == "Lanes"){
      if(LaneMeshesIter != LaneMeshes.end()){
        OGRLineString lineString_even;
        OGRLineString lineString_odd;
        std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
        std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());
        odr::Mesh3D LaneMesh = *LaneMeshesIter;
        odr::Lane Lane = *LaneIter;

        std::vector<odr::Vec3D> LaneGeometries = LaneMesh.vertices;
        for (int lanegeometry = 0; lanegeometry < LaneGeometries.size(); lanegeometry++) {
          odr::Vec3D LaneGeometry = LaneGeometries[lanegeometry];

          if (lanegeometry % 2 != 0) { 
            lineString_even.addPoint(LaneGeometry[0], LaneGeometry[1]);
          } else {
            lineString_odd.addPoint(LaneGeometry[0], LaneGeometry[1]);
          }
        }
        poLS->addGeometry(&lineString_even);
        poLS->addGeometry(&lineString_odd);
        std::unique_ptr<OGRGeometry> poGeometry(poLS->MakeValid());
        poFeature->SetGeometry(poGeometry.get());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("ID"), Lane.id);
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), Lane.type.c_str());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Predecessor"), Lane.predecessor);
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Successor"), Lane.successor);

        poFeature->SetFID(nNextFID++);
        LaneIter++;
        LaneMeshesIter++;
        if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
              (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
              return poFeature.release();
        }
      }
    } 
    /*---------------------------   RoadMark  ----------------------------*/
    if( layerName == "RoadMark"){
        if(RoadMarkMeshesIter != RoadMarkMeshes.end()){
          OGRLineString lineString_even;
          OGRLineString lineString_odd;
          std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
          std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());
          odr::Mesh3D RoadMarkMesh = *RoadMarkMeshesIter;
          odr::RoadMark RoadMark = *RoadMarkIter;

          std::vector<odr::Vec3D> RoadMarkGeometries = RoadMarkMesh.vertices;
          for (int roadmarkgeometry = 0; roadmarkgeometry < RoadMarkGeometries.size(); roadmarkgeometry++) {
            odr::Vec3D RoadMarkGeometry = RoadMarkGeometries[roadmarkgeometry];

            if (roadmarkgeometry % 2 != 0) { 
              lineString_even.addPoint(RoadMarkGeometry[0], RoadMarkGeometry[1]);
            } else {
              lineString_odd.addPoint(RoadMarkGeometry[0], RoadMarkGeometry[1]);
            }
          }
          poLS->addGeometry(&lineString_even);
          poLS->addGeometry(&lineString_odd);
          std::unique_ptr<OGRGeometry> poGeometry(poLS->MakeValid());
          poFeature->SetGeometry(poGeometry.get());
          poFeature->SetField(poFeatureDefn->GetFieldIndex("Road ID"), RoadMark.road_id.c_str());
          poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), RoadMark.type.c_str());
          poFeature->SetField(poFeatureDefn->GetFieldIndex("Lane ID"), RoadMark.lane_id);
          poFeature->SetFID(nNextFID++);
          RoadMarkIter++;
          RoadMarkMeshesIter++;
          if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
              (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
              return poFeature.release();
          }
        }
    }
    /*--------------------------- RoadObjects ----------------------------*/
    if (layerName == "RoadObject") {
      if (RoadObjectMeshesIter != RoadObjectMeshes.end()) {
        OGRLineString lineString;
        std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
        std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());
        const odr::Mesh3D& RoadObjectMesh = *RoadObjectMeshesIter;
        odr::RoadObject RoadObject = *RoadObjectIter;
        const std::vector<odr::Vec3D> RoadObjectGeometries = RoadObjectMesh.vertices;
 
        for (int roadobjectgeometry = 0; roadobjectgeometry < RoadObjectGeometries.size(); roadobjectgeometry++) {
          const odr::Vec3D& RoadObjectGeometry = RoadObjectGeometries[roadobjectgeometry];
          lineString.addPoint(RoadObjectGeometry[0], RoadObjectGeometry[1]);
        }
        poLS->addGeometry(&lineString);
        std::unique_ptr<OGRGeometry> poGeometry(poLS->MakeValid());
        poFeature->SetGeometry(poGeometry.get());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Object ID"), RoadObject.id.c_str());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), RoadObject.type.c_str());
        poFeature->SetField(poFeatureDefn->GetFieldIndex("Name"), RoadObject.name.c_str());

        poFeature->SetFID(nNextFID++);
        RoadObjectIter++;
        RoadObjectMeshesIter++;
        if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
          return poFeature.release();
        }
      }
  } 

    
  return nullptr;
}
std::vector<odr::LaneSection> OGRXODRLayer::getFullLaneSections(){
    std::vector<odr::LaneSection> LaneSections_;
    
    for(int road = 0; road < Roads.size(); road++) {
      odr::Road Road = Roads[road];
      std::vector<odr::LaneSection> LaneSections = Road.get_lanesections();
      for (int lanesection = 0; lanesection < LaneSections.size(); lanesection++) {
        odr::LaneSection LaneSection = LaneSections[lanesection];
        LaneSections_.push_back(LaneSection);
      }
    }
  return LaneSections_;
}
/*--------------------------------------------------------------------*/
/*---------------------------     Lane    ----------------------------*/
/*--------------------------------------------------------------------*/
std::vector<odr::Lane> OGRXODRLayer::getFullLanes(){
    std::vector<odr::Lane> Lanes_;
    
    for(int road = 0; road < Roads.size(); road++) {
      odr::Road Road = Roads[road];
      std::vector<odr::LaneSection> LaneSections = Road.get_lanesections();
      for (int lanesection = 0; lanesection < LaneSections.size(); lanesection++) {
        odr::LaneSection LaneSection = LaneSections[lanesection];
        std::vector<odr::Lane> Lanes = LaneSection.get_lanes();
        for (int lane = 0; lane < Lanes.size(); lane++) {
          odr::Lane Lane = Lanes[lane];
          Lanes_.push_back(Lane);
        }
      }
    }
  return Lanes_;
}
std::vector<odr::Mesh3D> OGRXODRLayer::getFullLaneMeshes(){
    std::vector<odr::Mesh3D> LaneMeshes_;
    const double eps = 0.5;
    for(int road = 0; road < Roads.size(); road++) {
      odr::Road Road = Roads[road];
      std::vector<odr::LaneSection> LaneSections = Road.get_lanesections();
      for (int lanesection = 0; lanesection < LaneSections.size(); lanesection++) {
        odr::LaneSection LaneSection = LaneSections[lanesection];
        std::vector<odr::Lane> Lanes = LaneSection.get_lanes();
        for (int lane = 0; lane < Lanes.size(); lane++) {
          odr::Lane& Lane = Lanes[lane];
          odr::Mesh3D LaneMesh = Road.get_lane_mesh(Lane, eps);
          LaneMeshes_.push_back(LaneMesh);
        }
      }
    }
  return LaneMeshes_;
}
/*--------------------------------------------------------------------*/
/*---------------------------   RoadMark  ----------------------------*/
/*--------------------------------------------------------------------*/

std::vector<odr::RoadMark> OGRXODRLayer::getFullRoadMarks() {
  std::vector<odr::RoadMark> RoadMark_;
  const double eps = 0.9;

  for (int road = 0; road < Roads.size(); road++) {
    odr::Road& Road = Roads[road];
    const std::vector<odr::LaneSection>& LaneSections = Road.get_lanesections();

    for (int lanesection = 0; lanesection < LaneSections.size(); lanesection++) {
      const odr::LaneSection& LaneSection = LaneSections[lanesection];
      const std::vector<odr::Lane>& Lanes = LaneSection.get_lanes();

      for (int lane = 0; lane < Lanes.size(); lane++) {
        const odr::Lane& Lane = Lanes[lane];
        const std::vector<RoadMark>& RoadMarks = Lane.get_roadmarks(LaneSection.s0, Road.get_lanesection_end(LaneSection));

        for (int roadmark = 0; roadmark < RoadMarks.size(); roadmark++) {
          const odr::RoadMark& RoadMark = RoadMarks[roadmark];
          RoadMark_.push_back(std::move(RoadMark));
        }
      }
    }
  }

  return RoadMark_;
}

std::vector<odr::Mesh3D> OGRXODRLayer::getFullRoadMarkMeshes() {
  std::vector<odr::Mesh3D> RoadMarkMeshes_;
  const double eps = 0.9;

  for (int road = 0; road < Roads.size(); road++) {
    odr::Road& Road = Roads[road];
    const std::vector<odr::LaneSection>& LaneSections = Road.get_lanesections();

    for (int lanesection = 0; lanesection < LaneSections.size(); lanesection++) {
      const odr::LaneSection& LaneSection = LaneSections[lanesection];
      const std::vector<odr::Lane>& Lanes = LaneSection.get_lanes();

      for (int lane = 0; lane < Lanes.size(); lane++) {
        const odr::Lane& Lane = Lanes[lane];
        const std::vector<RoadMark>& RoadMarks = Lane.get_roadmarks(LaneSection.s0, Road.get_lanesection_end(LaneSection));

        for (int roadmark = 0; roadmark < RoadMarks.size(); roadmark++) {
          const odr::RoadMark& RoadMark = RoadMarks[roadmark];
          odr::Mesh3D RoadMarkMesh = Road.get_roadmark_mesh(Lane, RoadMark, eps);
          RoadMarkMeshes_.push_back(std::move(RoadMarkMesh));
        }
      }
    }
  }

  return RoadMarkMeshes_;
}


/*--------------------------------------------------------------------*/
/*--------------------------- RoadObjects ----------------------------*/
/*--------------------------------------------------------------------*/ 

std::vector<odr::RoadObject> OGRXODRLayer::getFullRoadObjects(){
  std::vector<odr::RoadObject> RoadObjects_;
  
  const double eps = 0.9;
  for(int road = 0; road < Roads.size(); road++) {
    odr::Road Road = Roads[road];
    std::vector<odr::RoadObject> RoadObjects = Road.get_road_objects();
    for(int roadobject = 0; roadobject < RoadObjects.size(); roadobject ++){
      odr::RoadObject RoadObject = RoadObjects[roadobject];
      
      RoadObjects_.push_back(RoadObject);
    }
  }  
  return RoadObjects_;
  
}

std::vector<odr::Mesh3D> OGRXODRLayer::getFullRoadObjectMeshes(){
  std::vector<odr::Mesh3D> RoadObjectMeshes_;
  
  const double eps = 0.9;
  for(int road = 0; road < Roads.size(); road++) {
    odr::Road Road = Roads[road];
    std::vector<odr::RoadObject> RoadObjects = Road.get_road_objects();
    for(int roadobject = 0; roadobject < RoadObjects.size(); roadobject ++){
      odr::RoadObject RoadObject = RoadObjects[roadobject];
      odr::Mesh3D RoadObjectMesh = Road.get_road_object_mesh(RoadObject, eps);
      RoadObjectMeshes_.push_back(RoadObjectMesh);
    }
  }  
  return RoadObjectMeshes_;
  
}

/*--------------------------------------------------------------------*/
/*--------------------       Reset Reading      ----------------------*/
/*--------------------------------------------------------------------*/
void OGRXODRLayer::ResetReading() {
  VSIFSeekL(fpXODR, 0, SEEK_SET);
  nNextFID = 0;
}
/*--------------------------------------------------------------------*/
/*---------------------------     Layer    ----------------------------*/
/*--------------------------------------------------------------------*/


OGRFeature *OGRXODRLayer::getLayer() {

  if (layerName == "refLine") { // reference line
    OGRFieldDefn oFieldID("ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldID);
    OGRFieldDefn oFieldLen("Length", OFTReal);
    poFeatureDefn->AddFieldDefn(&oFieldLen);
    OGRFieldDefn oFieldJunction("Junction", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldJunction);
  } else if (layerName == "Lanes") {
    OGRFieldDefn oFieldID("ID", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldID);
    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);
    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);
    OGRFieldDefn oFieldPred("Predecessor", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldPred);
    OGRFieldDefn oFieldSuc("Successor", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldSuc);
  } else if (layerName == "RoadMark") {
    OGRFieldDefn oFieldRoadID("Road ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);
    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);
    OGRFieldDefn oFieldLaneID("Lane ID", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldLaneID);
  } else if (layerName == "RoadObject") {
    OGRFieldDefn oFieldRoadID("Object ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);
    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);
    OGRFieldDefn oFieldObjectName("Name", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldObjectName);
  }

  return NULL;
}