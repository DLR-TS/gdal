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
#include "geos/geom/Point.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/LineString.h"
#include "Road.h"

#include <vector>
/*
class OGRXODRLayer : public OGRLayer {
protected:
	XODR* xodr;
	// TODO Define simpleRoad
    //std::vector<odr::Road> simpleRoads;
	// TODO Define iterate road
    //std::vector<odr::Vec3D>::iterator roadIter;
	OGRFeatureDefn* poFeatureDefn;
	OGRSpatialReference *poSRS;
	int nNextFID;

public:
	// TODO Define Layer
    //OGRXODRLayer(XODR *xodr, const std::vector<odr::Vec3D> &roads, const char* pszName, uint8_t lId);

	int roadElement;
	uint8_t layerId;

	~OGRXODRLayer();
	bool print = false;
	void ResetReading();

	OGRFeature *GetNextFeature();

	OGRFeatureDefn *GetLayerDefn()
	{
		return poFeatureDefn;
	}

	int TestCapability(const char *) {
		return false;
	}

private:
	enum layerValue {
		refLayer=0,
		laneLayer=1,
		centerLanelayer=2,
		objectLayer=3,
		objectPolyLayer=4,
		signalLayer=5,
		markLayer=6
	};

	OGRFeature* getReferenceLineFeatures();
    //TODO Define features
	//OGRFeature* getLaneLineFeatures();
	//OGRFeature* getCenterLaneLineFeatures();
	//OGRFeature* getRoadMarkLineFeatures();
	//OGRFeature* getSignalFeatures();
	//OGRFeature* getObjectFeatures();
	//void createRequestedLayer();

	int featureIndex = 0;
    // Define layer indexes
	// std::vector<LaneSF> currentLanes;
	// std::vector<CenterLaneSF*> currentCenterLanes;
	// std::vector<ObjectSF> currentObjects;
	// std::vector<SignalSF> currentSignals;


class OGRXODRDataSource : public GDALDataset {
	OGRXODRLayer **papoLayers;
	int nLayers;
	XODR* xodr;

public:

	OGRXODRDataSource();

	~OGRXODRDataSource();

	int Open(GDALOpenInfo* openInfo);

	int GetLayerCount()
	{
		return nLayers;
	}

	OGRLayer* GetLayer(int);

	int TestCapability(const char*);
};

}
*/