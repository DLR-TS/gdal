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
  private:
    VSILFILE *file;
    std::string layerName;
    std::vector<odr::Road> roads;
    OGRSpatialReference *spatialRef;

    /* Unique feature ID which is automatically incremented for any new road feature creation. */
    int nNextFID;

    struct RoadElements
    {
        std::vector<odr::Lane> lanes;
        std::vector<odr::LaneSection> laneSections;
        std::vector<std::string> laneRoadIDs;
        std::vector<odr::Mesh3D> laneMeshes;

        std::vector<odr::Line3D> laneLinesInner;
        std::vector<odr::Line3D> laneLinesOuter;
        
        std::vector<odr::RoadMark> roadMarks;
        std::vector<odr::Mesh3D> roadMarkMeshes;

        std::vector<odr::RoadObject> roadObjects;
        std::vector<odr::Mesh3D> roadObjectMeshes;
    };
    RoadElements roadElements;

    std::vector<odr::Road>::iterator roadIter;

    std::vector<odr::Lane>::iterator laneIter;
    std::vector<odr::LaneSection>::iterator laneSectionIter;
    std::vector<std::string>::iterator laneRoadIDIter;
    std::vector<odr::Mesh3D>::iterator laneMeshIter;
    
    std::vector<odr::Line3D>::iterator laneLinesInnerIter;
    std::vector<odr::Line3D>::iterator laneLinesOuterIter;
    
    std::vector<odr::RoadMark>::iterator roadMarkIter;
    std::vector<odr::Mesh3D>::iterator roadMarkMeshIter;

    std::vector<odr::RoadObject>::iterator roadObjectIter;
    std::vector<odr::Mesh3D>::iterator roadObjectMeshesIter;
   
    OGRFeatureDefn *featureDefn;

    virtual void ResetReading() override;
    virtual OGRFeature *GetNextFeature() override;

    virtual OGRFeatureDefn *GetLayerDefn() override
    {
        return featureDefn;
    }
    virtual int TestCapability(const char *) override
    {
        return FALSE;
    }

    /**
     * Initializes XODR road elements and iterators.
    */
    void resetRoadElementIterators();

    /**
     * Completes feature class definition with all specific attributes and geometry type
     * according to layer type.
    */
    void defineFeatureClass();
    
    /**
     * Retrieves all necessary road elements from the underlying OpenDRIVE structure.
     * 
     * \param xodrRoads Roads of the dataset.
     * \param eps Approximation factor for sampling of continuous geometry functions into discrete
     * OGC Simple Feature geometries.
    */
    RoadElements createRoadElements(const double eps = 0.5);

  public:
    OGRXODRLayer(VSILFILE *filePtr, std::string name,
                 std::vector<odr::Road> xodrRoads, std::string proj4Defn);
    ~OGRXODRLayer();
};

/*--------------------------------------------------------------------*/
/*--------------------  Data source declarations ----------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRDataSource : public GDALDataset
{
  private:
    OGRXODRLayer **layers;
    int nLayers;

  public:
    OGRXODRDataSource();
    ~OGRXODRDataSource();

    int Open(const char *fileName, int bUpdate);

    int GetLayerCount() override
    {
        return nLayers;
    }

    OGRLayer *GetLayer(int) override;

    virtual int TestCapability(const char *) override;
};
