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
 

#pragma pack(pop)
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include <iostream>
#include <OpenDriveMap.h>
#include <pugixml/pugixml.hpp>
#include <vector>


/*--------------------------------------------------------------------*/
/*---------------------  Layer declerations  -------------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRLayer : public OGRLayer
{
    OGRFeatureDefn          *poFeatureDefn;
    VSILFILE                *fpXODR;
    char                    *pszFilename;
    int                      nNextFID;
    OGRSpatialReference     *poSRS;
    

public:
    OGRXODRLayer( const char *pszFilename, VSILFILE *fp,  const char *pszLayerName, std::string layer, std::vector<odr::Road> Roads, std::string refSystem);
~OGRXODRLayer();

    std::string                                 fileName;
    std::string                                 layerName;
    std::vector<odr::Road>                      Roads;
    std::vector<odr::LaneSection>               LaneSections;
    std::vector<odr::Lane>                      Lanes;
    std::vector<odr::RoadMark>                  RoadMarks;
    std::vector<odr::RoadObject>                RoadObjects;

    std::vector<odr::Mesh3D>                    LaneMeshes;
    std::vector<odr::Mesh3D>                    RoadMarkMeshes;
    std::vector<odr::Mesh3D>                    RoadObjectMeshes;

    std::vector<odr::Road>::iterator            RoadIter;
    std::vector<odr::Lane>::iterator            LaneIter;
    std::vector<odr::LaneSection>::iterator     LaneSectionIter;
    std::vector<odr::RoadMark>::iterator        RoadMarkIter;
    std::vector<odr::RoadObject>::iterator      RoadObjectIter;

    std::vector<odr::Mesh3D>::iterator          LaneMeshesIter;
    std::vector<odr::Mesh3D>::iterator          RoadMarkMeshesIter;
    std::vector<odr::Mesh3D>::iterator          RoadObjectMeshesIter;

    void                                        ResetReading();
    OGRFeature *                                GetNextFeature();

    OGRFeatureDefn *                            GetLayerDefn() {return poFeatureDefn; }
    int                                         TestCapability( const char * ) { return FALSE; }
    
private:
    enum layer_value {
        header = 0
    };
    
    std::string                         getReferenceSystem();
    OGRFeature*                         getLayer();

    std::vector<odr::LaneSection>       getFullLaneSections();
    std::vector<odr::Lane>              getFullLanes();
    std::vector<odr::RoadMark>          getFullRoadMarks();
    std::vector<odr::RoadObject>          getFullRoadObjects();

    std::vector<odr::Mesh3D>            getFullLaneMeshes();
    std::vector<odr::Mesh3D>            getFullRoadMarkMeshes();
    std::vector<odr::Mesh3D>            getFullRoadObjectMeshes();
 };

/*--------------------------------------------------------------------*/
/*--------------------  Datasource declerations ----------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRDataSource : public GDALDataset
{
    OGRXODRLayer       **papoLayers;
    int                 nLayers;
    std::string         fileName;

public:
                        OGRXODRDataSource();
                        ~OGRXODRDataSource();

    int                 Open(const char *pszFilename, int bUpdate);

    int                 GetLayerCount() { return nLayers; }
    OGRLayer            *GetLayer( int );

    virtual int         TestCapability( const char * ) override;
    std::vector<odr::Road>    roads;
};

/*--------------------------------------------------------------------*/
/*--------------------    Driver declerations   ----------------------*/
/*--------------------------------------------------------------------*/

static GDALDataset* OGRXODRDriverOpen(GDALOpenInfo* poOpenInfo);
static int          OGRXODRDriverIdentify(GDALOpenInfo* poOpenInfo);
static GDALDataset* OGRXODRDriverCreate(const char* pszLayerName, int nXSize, int nYSize,
                                    int nBands, GDALDataType eDT, char** papszOptions);

/*--------------------------------------------------------------------*/