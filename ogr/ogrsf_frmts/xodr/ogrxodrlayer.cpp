
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
    pszFilename_(CPLStrdup(pszFilename)),
    fpXODR(fp), 
    Roads(roads),
    RoadIter(Roads.begin()),
    nNextFID(0),
    poSRS(NULL){

  layerName = layer;
  Initialization();

  poFeatureDefn = new OGRFeatureDefn(CPLGetBasename(pszLayerName));
  SetDescription(poFeatureDefn->GetName());
  
  if (layerName == "ReferenceLine" || layerName == "LaneBorder" ||
      layerName == "RoadMark" ||
      layerName == "RoadObject") {
    
    poFeatureDefn->SetGeomType(wkbMultiLineString);
  }else if(layerName == "Lane"){
    poFeatureDefn->SetGeomType(wkbMultiPolygon);
  }
  
  poFeatureDefn->Reference();
  ResetReading();
  poSRS = new OGRSpatialReference();
  poSRS->importFromProj4(refSystem.c_str());
  poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);

  getLayer();

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
    if(layerName == "ReferenceLine"){
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
    if((layerName == "LaneBorder") && (LaneMeshesIter != LaneMeshes.end())){
      
      OGRLineString lineString_even;
      OGRLineString lineString_odd;
      std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
      std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());

      odr::Mesh3D LaneMesh = *LaneMeshesIter;
      odr::Lane Lane = *LaneIter;
      std::string LaneRoadID = *LanesRoadIDsIter;

      std::vector<odr::Vec3D> LaneGeometries = LaneMesh.vertices;
      for (std::size_t lanegeometry = 0; lanegeometry < LaneGeometries.size(); lanegeometry++) {
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

      poFeature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), LaneRoadID.c_str());
      poFeature->SetField(poFeatureDefn->GetFieldIndex("ID"), Lane.id);
      poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), Lane.type.c_str());
      poFeature->SetField(poFeatureDefn->GetFieldIndex("Predecessor"), Lane.predecessor);
      poFeature->SetField(poFeatureDefn->GetFieldIndex("Successor"), Lane.successor);

      poFeature->SetFID(nNextFID++);

      LanesRoadIDsIter++;
      LaneIter++;
      LaneMeshesIter++;

      if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
            return poFeature.release();
      }
    } 
    /*---------------------------   RoadMark  ----------------------------*/
    if( (layerName == "RoadMark") && (RoadMarkMeshesIter != RoadMarkMeshes.end())){
        
      OGRLineString lineString_even;
      OGRLineString lineString_odd;

      std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
      std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());

      odr::Mesh3D RoadMarkMesh = *RoadMarkMeshesIter;
      odr::RoadMark RoadMark = *RoadMarkIter;

      std::vector<odr::Vec3D> RoadMarkGeometries = RoadMarkMesh.vertices;
      for (std::size_t roadmarkgeometry = 0; roadmarkgeometry < RoadMarkGeometries.size(); roadmarkgeometry++) {
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
      poFeature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), RoadMark.road_id.c_str());
      poFeature->SetField(poFeatureDefn->GetFieldIndex("LaneID"), RoadMark.lane_id);
      poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), RoadMark.type.c_str());
      
      poFeature->SetFID(nNextFID++);

      RoadMarkIter++;
      RoadMarkMeshesIter++;

      if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
          (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
          return poFeature.release();
      }
        
    }
    /*--------------------------- RoadObjects ----------------------------*/
    if ((layerName == "RoadObject") && (RoadObjectMeshesIter != RoadObjectMeshes.end())) {
     
      OGRLineString lineString;
      std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
      std::unique_ptr<OGRMultiLineString> poLS(new OGRMultiLineString());
      const odr::Mesh3D& RoadObjectMesh = *RoadObjectMeshesIter;
      odr::RoadObject RoadObject = *RoadObjectIter;
      const std::vector<odr::Vec3D> RoadObjectGeometries = RoadObjectMesh.vertices;

      for (std::size_t roadobjectgeometry = 0; roadobjectgeometry < RoadObjectGeometries.size(); roadobjectgeometry++) {
        const odr::Vec3D& RoadObjectGeometry = RoadObjectGeometries[roadobjectgeometry];
        lineString.addPoint(RoadObjectGeometry[0], RoadObjectGeometry[1]);
      }
      poLS->addGeometry(&lineString);
      std::unique_ptr<OGRGeometry> poGeometry(poLS->MakeValid());
      poFeature->SetGeometry(poGeometry.get());
      poFeature->SetField(poFeatureDefn->GetFieldIndex("ObjectID"), RoadObject.id.c_str());
      poFeature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), RoadObject.road_id.c_str());
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
  /*--------------------------- Lanes ----------------------------*/
  if((layerName == "Lane") && (LaneMeshesIter != LaneMeshes.end())){
    OGRLineString lineString_even;
    OGRLineString lineString_odd;

    std::unique_ptr<OGRFeature> poFeature(new OGRFeature(poFeatureDefn));
    std::unique_ptr<OGRMultiPolygon> poMP(new OGRMultiPolygon());

    odr::Mesh3D LaneMesh = *LaneMeshesIter;
    odr::Lane Lane = *LaneIter;
    std::string LaneRoadID = *LanesRoadIDsIter;

    std::vector<odr::Vec3D> LaneGeometries = LaneMesh.vertices;
    for(std::size_t lanegeometry = 0; lanegeometry < LaneGeometries.size(); lanegeometry++){
      odr::Vec3D LaneGeometry = LaneGeometries[lanegeometry];
      if (lanegeometry % 2 != 0) { 
        lineString_even.addPoint(LaneGeometry[0], LaneGeometry[1]);
      } else {
        lineString_odd.addPoint(LaneGeometry[0], LaneGeometry[1]);
      }
    
    }

    OGRLinearRing ring;
    for (int i = 0; i < lineString_even.getNumPoints(); ++i) {
      OGRPoint point;
      lineString_even.getPoint(i, &point);
      ring.addPoint(point.getX(), point.getY());
    }
    for (int i = lineString_odd.getNumPoints() - 1; i >= 0; --i) {
        OGRPoint point;
        lineString_odd.getPoint(i, &point);
        ring.addPoint(point.getX(), point.getY());
    }
    OGRPoint startPoint;
    lineString_even.getPoint(0, &startPoint);
    ring.addPoint(startPoint.getX(), startPoint.getY());

    std::unique_ptr<OGRPolygon> poP(new OGRPolygon());
    poP->addRing(&ring);

    std::unique_ptr<OGRGeometry> poGeometry(poP->MakeValid());
    poFeature->SetGeometry(poGeometry.get());
    
    poFeature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), LaneRoadID.c_str());
    poFeature->SetField(poFeatureDefn->GetFieldIndex("LaneID"), Lane.id);
    poFeature->SetField(poFeatureDefn->GetFieldIndex("Type"), Lane.type.c_str());
    poFeature->SetField(poFeatureDefn->GetFieldIndex("Predecessor"), Lane.predecessor);
    poFeature->SetField(poFeatureDefn->GetFieldIndex("Successor"), Lane.successor);
      
    poFeature->SetFID(nNextFID++);

    LanesRoadIDsIter++;
    LaneIter++;
    LaneMeshesIter++; 

    if ((m_poFilterGeom == nullptr || FilterGeometry(poFeature->GetGeometryRef())) &&
            (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate(poFeature.get()))) {
            return poFeature.release();
    }
  } 

  return nullptr;
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

  if (layerName == "ReferenceLine") { // reference line

    OGRFieldDefn oFieldID("ID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldID);

    OGRFieldDefn oFieldLen("Length", OFTReal);
    poFeatureDefn->AddFieldDefn(&oFieldLen);

    OGRFieldDefn oFieldJunction("Junction", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldJunction);

  } else if (layerName == "LaneBorder") {
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

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldLaneID);

    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldName("Name", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldName);

  } else if (layerName == "RoadObject") {

    OGRFieldDefn oFieldObjectID("ObjectID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldObjectID);

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldObjectName("Name", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldObjectName);

  } else if (layerName == "Lane") {

    OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldLaneID);

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldType("Type", OFTString);
    poFeatureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldPred("Predecessor", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldPred);

    OGRFieldDefn oFieldSuc("Successor", OFTInteger);
    poFeatureDefn->AddFieldDefn(&oFieldSuc);

  }


  return NULL;
}
/*--------------------------------------------------------------------*/
/*------------------------ Initialization   --------------------------*/
/*--------------------------------------------------------------------*/
// This function allows to initialize public variable to setting iteration correctly element by element
void OGRXODRLayer::Initialization(){
  RoadElements roadElements = getRoadElements();
   
  LaneSections = roadElements.laneSections;
  LaneSectionIter = LaneSections.begin();

  Lanes = roadElements.lanes;
  LaneIter = Lanes.begin();

  LanesRoadIDs = roadElements.laneRoadID;
  LanesRoadIDsIter = LanesRoadIDs.begin();

  LaneMeshes = roadElements.laneMeshes;
  LaneMeshesIter = LaneMeshes.begin();

  
  RoadMarks = roadElements.roadMarks;
  RoadMarkIter = RoadMarks.begin();

  RoadMarkMeshes = roadElements.roadMarkMeshes;
  RoadMarkMeshesIter = RoadMarkMeshes.begin();

  RoadObjects = roadElements.roadObjects;
  RoadObjectIter = RoadObjects.begin();

  RoadObjectMeshes = roadElements.roadObjectMeshes;
  RoadObjectMeshesIter = RoadObjectMeshes.begin();


}


/*--------------------------------------------------------------------*/
/*------------------------ getRoadElements   --------------------------*/
/*--------------------------------------------------------------------*/
// This function provide every sub-element of road as vector 
OGRXODRLayer::RoadElements OGRXODRLayer::getRoadElements(){
  RoadElements roadElements;

  const double eps = 0.5;
  //for (int road = 0; road < Roads.size(); road++) {
  for (odr::Road Road : Roads) {

    for (odr::LaneSection LaneSection : Road.get_lanesections()) {

      roadElements.laneSections.push_back(LaneSection);

      for (odr::Lane Lane : LaneSection.get_lanes()) {
        roadElements.laneRoadID.push_back(Road.id); 
      

        roadElements.lanes.push_back(Lane); 

        odr::Mesh3D LaneMesh = Road.get_lane_mesh(Lane, eps);
        roadElements.laneMeshes.push_back(LaneMesh);


        for(odr::RoadMark RoadMark : Lane.get_roadmarks(LaneSection.s0, Road.get_lanesection_end(LaneSection))){

          roadElements.roadMarks.push_back(RoadMark);

          odr::Mesh3D RoadMarkMesh = Road.get_roadmark_mesh(Lane, RoadMark, eps);
          roadElements.roadMarkMeshes.push_back(RoadMarkMesh);
        }
      }
    }

    for(odr::RoadObject RoadObject: Road.get_road_objects()){  
      roadElements.roadObjects.push_back(RoadObject);

      odr::Mesh3D RoadObjectMesh = Road.get_road_object_mesh(RoadObject, eps);
      roadElements.roadObjectMeshes.push_back(RoadObjectMesh);
    }

  }
  return roadElements;
}