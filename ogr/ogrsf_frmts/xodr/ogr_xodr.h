/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of OGR driver components for OpenDRIVE.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2024 German Aerospace Center (DLR), Institute of Transportation Systems
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#pragma once
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include <iostream>
#include <OpenDriveMap.h>
#include <pugixml/pugixml.hpp>
#include <vector>

struct RoadElements
{
    /* Map of road to its original OpenDRIVE ID for fast lookup. */
    std::map<std::string, odr::Road> roads;
    std::vector<odr::Line3D> referenceLines;

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

    std::vector<odr::RoadSignal> roadSignals;
    std::vector<odr::Mesh3D> roadSignalMeshes;
};

/*--------------------------------------------------------------------*/
/*---------------------  Layer declarations  -------------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRLayer : public OGRLayer
{
  private:
    virtual void ResetReading() override;

    virtual OGRFeatureDefn *GetLayerDefn() override
    {
        return  m_poFeatureDefn;//featureDefn;
    }

    virtual int TestCapability(const char *) override
    {
        return FALSE;
    }

    /**
     * Initializes XODR road elements and iterators.
    */
    void resetRoadElementIterators();
  
  protected:
    //VSILFILE *m_file; -> unneccessary 
    OGRSpatialReference m_poSRS;
    OGRFeatureDefn *m_poFeatureDefn;

    RoadElements m_roadElements;
    bool m_bDissolveTIN;

    /* Unique feature ID which is automatically incremented for any new road feature creation. */
    int m_nNextFID;

    std::map<std::string, odr::Road>::iterator roadIter;
    std::vector<odr::Line3D>::iterator referenceLineIter;

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

    std::vector<odr::RoadSignal>::iterator roadSignalIter;
    std::vector<odr::Mesh3D>::iterator roadSignalMeshesIter;

    /**
     * Completes feature class definition with all specific attributes and geometry type
     * according to layer type.
    */
    virtual void defineFeatureClass() = 0;

    /**
     * Builds an ordinary TIN from libOpenDRIVE's mesh.
    */
    std::unique_ptr<OGRTriangulatedSurface> triangulateSurface(odr::Mesh3D mesh);

  public:
    /**
     * \param dissolveTriangulatedSurface True if original triangulated surface meshes from 
     * libOpenDRIVE are to be dissolved into simpler geometries.
     * Only applicable for layer types derived from meshes.
    */
    // TODO For lower memory consumption maybe better pass xodrRoadElements by reference?
    OGRXODRLayer(const RoadElements &xodrRoadElements, std::string proj4Defn);
    OGRXODRLayer(const RoadElements &xodrRoadElements, std::string proj4Defn,
                 bool dissolveTriangulatedSurface);
    ~OGRXODRLayer();
};

class OGRXODRLayerReferenceLine : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "ReferenceLine";

    OGRXODRLayerReferenceLine(const RoadElements& xodrRoadElements,
                              std::string proj4Defn);
};

class OGRXODRLayerLaneBorder : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "LaneBorder";

    OGRXODRLayerLaneBorder(const RoadElements& xodrRoadElements,
                           std::string proj4Defn);
};

class OGRXODRLayerRoadMark : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "RoadMark";

    OGRXODRLayerRoadMark(const RoadElements& xodrRoadElements, std::string proj4Defn,
                         bool dissolveTriangulatedSurface);
};

class OGRXODRLayerRoadObject : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "RoadObject";

    OGRXODRLayerRoadObject(const RoadElements& xodrRoadElements,
                           std::string proj4Defn);
};

class OGRXODRLayerRoadSignal : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "RoadSignal";

    OGRXODRLayerRoadSignal(const RoadElements& xodrRoadElements, std::string proj4Defn,
                           bool dissolveTriangulatedSurface);
};

class OGRXODRLayerLane : public OGRXODRLayer
{
  protected:
    virtual void defineFeatureClass() override;
    virtual OGRFeature *GetNextFeature() override;

  public:
    const std::string FEATURE_CLASS_NAME = "Lane";

    OGRXODRLayerLane(const RoadElements& xodrRoadElements, std::string proj4Defn,
                     bool dissolveTriangulatedSurface);
};

/*--------------------------------------------------------------------*/
/*--------------------  Data source declarations ----------------------*/
/*--------------------------------------------------------------------*/

class OGRXODRDataSource : public GDALDataset
{
  private:
    //OGRXODRLayer **m_apoLayers;
    //int nLayers;
    std::vector<std::unique_ptr<OGRXODRLayer>> m_apoLayers{};
    /**
     * Retrieves all necessary road elements from the underlying OpenDRIVE structure.
     * 
     * \param roads Roads of the dataset.
    */
    RoadElements createRoadElements(const std::vector<odr::Road>& roads);

  public:
    OGRXODRDataSource();
    ~OGRXODRDataSource();

    /**
     * Approximation factor for sampling of continuous geometry functions into discrete
     * OGC Simple Feature geometries.
    */
    double eps;

    /**
     * Whether to dissolve triangulated surfaces which are created from libOpenDRIVE's meshes.
    */
    bool m_bDissolveTIN;

    bool Open(const char *pszFilename, char **papszOpenOptions);

    int GetLayerCount() override
    {
        return m_apoLayers.size();
    }

    OGRLayer *GetLayer(int) override;

    virtual int TestCapability(const char *) override;
};
