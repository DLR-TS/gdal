#include "ogr_xodr.h"

OGRXODRDataSource::OGRXODRDataSource() :
	papoLayers(NULL),
	nLayers(0),
	xodr(NULL)
{
}

OGRXODRDataSource::~OGRXODRDataSource()
{
	for (int i = 0; i < nLayers; i++)
		delete papoLayers[i];

	CPLFree(papoLayers);

	if (xodr)
		delete xodr;
}

int OGRXODRDataSource::Open(GDALOpenInfo * openInfo)
{
	int bUpdate = openInfo->eAccess == GA_Update;
	if (bUpdate) {
		CPLError(CE_Failure, CPLE_OpenFailed, "Update access not supported by XODR driver.");
		return FALSE;
	}

	xodr = new XODR(openInfo->pszFilename);
	std::vector<RoadSF> roads = xodr->getRoads();
	nLayers = 5;
	papoLayers = (OGRXODRLayer **)CPLRealloc(papoLayers, sizeof(OGRXODRLayer *) * nLayers);

	// create the layers (xodrObject, opendriveRoadList, layerName, layerID)
	// layerID can be found in ogr_xodr.h
	std::string layerName = "referenceLine";
	papoLayers[0] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 0);
	layerName = "laneBorder";
	papoLayers[1] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 1);
	layerName = "centerLaneBorder";
	papoLayers[2] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 2);
	layerName = "object";
	papoLayers[3] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 3);
	layerName = "signal";
	papoLayers[4] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 5);
	//layerName = "roadMark";
	//papoLayers[5] = new OGRXODRLayer(xodr, layerName.c_str(), layerName);


	return TRUE;
}

OGRLayer * OGRXODRDataSource::GetLayer(int iLayer)
{
	if (iLayer < 0 || iLayer >= nLayers)
		return NULL;
	else
		return papoLayers[iLayer];
}

int OGRXODRDataSource::TestCapability(const char *capability)
{
	if (EQUAL(capability, ODsCCreateLayer))
		return TRUE;
	else
		return FALSE;
}
