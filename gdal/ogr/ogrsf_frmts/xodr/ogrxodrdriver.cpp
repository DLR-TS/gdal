/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRDriver.
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
#include "cpl_conv.h"
#include "cpl_error.h"

extern "C" void CPL_DLL RegisterOGRXODR();


static int OGRXODRDriverIdentity(GDALOpenInfo* openInfo)
{
	return EQUAL(CPLGetExtension(openInfo->pszFilename), "xodr");
}

static GDALDataset* OGRXODRDriverOpen(GDALOpenInfo* openInfo)
{
	if (!OGRXODRDriverIdentity(openInfo))
		return NULL;

	OGRXODRDataSource* ds = new OGRXODRDataSource();
	if (!ds->Open(openInfo)) 
	{
		delete ds;
		return NULL;
	}
	return ds;
}

// Register driver in OGR environment
void RegisterOGRXODR() 
{
    GDALDriver *driver;

    if (GDALGetDriverByName("XODR") == NULL) {
        driver = new GDALDriver();

        // set driver description
        driver->SetDescription("XODR");
        driver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
        driver->SetMetadataItem(GDAL_DMD_LONGNAME, "OpenDRIVE road network logic description");
        driver->SetMetadataItem(GDAL_DMD_EXTENSION, "xodr");
        driver->SetMetadataItem(GDAL_DMD_HELPTOPIC, "drv_xodr.html");

        driver->pfnOpen = OGRXODRDriverOpen;
        driver->pfnIdentify = OGRXODRDriverIdentity;

        GetGDALDriverManager()->RegisterDriver(driver);
    }
}
