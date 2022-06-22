#pragma once
#ifndef _OGR_XODR_H_INCLUDED
#define _OGR_XODR_H_INCLUDED

#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "geos/geom/Point.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/LineString.h"
#include "xodr.h"
#include "roadsf.h"
#include "LaneSF.h"
#include <vector>

#define USE_UNSTABLE_GEOS_CPP_API

class OGRXODRLayer : public OGRLayer {
protected:
	XODR* xodr;
	std::vector<RoadSF> simpleRoads;
	std::vector<RoadSF>::iterator roadIter;
	OGRFeatureDefn* poFeatureDefn;
	OGRSpatialReference *poSRS;
	int nNextFID;

public:
	// OGRXODRLayer(XODR *xodr, const char* pszName, uint8_t lId);
	OGRXODRLayer(XODR *xodr, const std::vector<RoadSF> &roads, const char* pszName, uint8_t lId);

	int roadElement;
	//std::string layerName;
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

	//OGRFeature* objectFeaturePolygon();
	//OGRFeature* objectFeatureLine();

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
	OGRFeature* getLaneLineFeatures();
	OGRFeature* getCenterLaneLineFeatures();
	//OGRFeature* getRoadMarkLineFeatures();
	OGRFeature* getSignalFeatures();
	OGRFeature* getObjectFeatures();
	void createRequestedLayer();

	int featureIndex = 0;
	std::vector<LaneSF> currentLanes;
	std::vector<CenterLaneSF*> currentCenterLanes;
	std::vector<ObjectSF> currentObjects;
	std::vector<SignalSF> currentSignals;
};

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


#endif /* ndef _OGR_XODR_H_INCLUDED */
