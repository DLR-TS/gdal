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
#include <OpenDriveMap.h>
#include <pugixml/pugixml.hpp>
#include <vector>


/*--------------------------------------------------------------------*/
/*---------------------  Layer declarations  -------------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRLayer : public OGRLayer
{
    const char              *pszFilename_;
    VSILFILE                *fpXODR;
    std::vector<odr::Road>  Roads;
    std::vector<odr::Road>::iterator RoadIter;
    int                      nNextFID;
    OGRSpatialReference     *poSRS;
    OGRFeatureDefn          *poFeatureDefn;
    
struct RoadElements{
  std::vector<odr::LaneSection> laneSections;
  
  std::vector<std::string>      laneRoadID;
  std::vector<odr::Lane>        lanes;
  std::vector<odr::Mesh3D>      laneMeshes;

  std::vector<odr::RoadMark>    roadMarks;
  std::vector<odr::Mesh3D>      roadMarkMeshes;

  std::vector<odr::RoadObject> roadObjects;
  std::vector<odr::Mesh3D>     roadObjectMeshes;
  
};
public:
    OGRXODRLayer( const char *pszFilename, VSILFILE *fp,  const char *pszLayerName, std::string layer, std::vector<odr::Road> Roads, std::string refSystem);
~OGRXODRLayer();

    std::string                                 fileName;
    std::string                                 layerName;


    std::vector<odr::LaneSection>               LaneSections;
    std::vector<odr::Lane>                      Lanes;
    std::vector<odr::RoadMark>                  RoadMarks;
    std::vector<odr::RoadObject>                RoadObjects;


    std::vector<std::string>                    LanesRoadIDs;
    std::vector<odr::Mesh3D>                    LaneMeshes;
    std::vector<odr::Mesh3D>                    RoadMarkMeshes;
    std::vector<odr::Mesh3D>                    RoadObjectMeshes;



    
    std::vector<odr::Lane>::iterator            LaneIter;
    std::vector<std::string>::iterator          LanesRoadIDsIter;
    std::vector<odr::LaneSection>::iterator     LaneSectionIter;
    std::vector<odr::RoadMark>::iterator        RoadMarkIter;
    std::vector<odr::RoadObject>::iterator      RoadObjectIter;


    std::vector<odr::Mesh3D>::iterator          LaneMeshesIter;
    std::vector<odr::Mesh3D>::iterator          RoadMarkMeshesIter;
    std::vector<odr::Mesh3D>::iterator          RoadObjectMeshesIter;


    virtual void                                ResetReading() override;
    virtual OGRFeature *                        GetNextFeature() override;

    virtual OGRFeatureDefn *                    GetLayerDefn() override {return poFeatureDefn; }
    virtual int                                 TestCapability( const char * ) override { return FALSE; }
    
private:
    enum layer_value {
        header = 0
    };
    
    std::string                         getReferenceSystem();
    OGRFeature*                         getLayer();

    void                                Initialization();
    RoadElements                        getRoadElements();
 };

/*--------------------------------------------------------------------*/
/*--------------------  Data source declarations ----------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRDataSource : public GDALDataset
{
    OGRXODRLayer       **papoLayers;
    int                 nLayers;

public:
                        OGRXODRDataSource();
                        ~OGRXODRDataSource();

    int                 Open(const char *fileName, int bUpdate);

    int                 GetLayerCount() override { return nLayers; }
    OGRLayer            *GetLayer( int ) override;

    virtual int         TestCapability( const char * ) override;
    std::vector<odr::Road>    roads;
};

/*--------------------------------------------------------------------*/
/*--------------------    Driver declarations   ----------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/