/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Created:  - 2017
 * Modified: February 2023
 * Purpose:  Implementation of OGRXODRDataSource.
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
/*
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
    // TODO convert RoadSF to road
	//std::vector<RoadSF> roads = xodr->getRoads();
	nLayers = 5;
	papoLayers = (OGRXODRLayer **)CPLRealloc(papoLayers, sizeof(OGRXODRLayer *) * nLayers);

	// create the layers (xodrObject, opendriveRoadList, layerName, layerID)
	// layerID can be found in ogr_xodr.h
    // TODO add layers 
	std::string layerName = "referenceLine";
	papoLayers[0] = new OGRXODRLayer(xodr, roads, layerName.c_str(), 0);

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
*/