/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRDriver.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2023 German Aerospace Center (DLR), Institute of Transportation Systems
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

CPL_CVSID("$Id$")  //TODO do we need this? Also the $Id$ in above licence headers?

extern "C" void CPL_DLL RegisterOGRXODR();

static GDALDataset *OGRXODRDriverOpen(GDALOpenInfo *openInfo)
{
    if (openInfo->eAccess == GA_Update || openInfo->fpL == nullptr)
        return nullptr;

    OGRXODRDataSource *dataSource = new OGRXODRDataSource();

    if (!dataSource->Open(openInfo->pszFilename, FALSE))
    {
        delete dataSource;
        dataSource = nullptr;
    }

    return dataSource;
}

static int OGRXODRDriverIdentity(GDALOpenInfo *openInfo)
{
    return EQUAL(CPLGetExtension(openInfo->pszFilename), "xodr");
}

void RegisterOGRXODR()
{
    if (GDALGetDriverByName("XODR") != nullptr)
        return;

    GDALDriver *driver = new GDALDriver();
    driver->SetDescription("XODR");
    driver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
    driver->SetMetadataItem(
        GDAL_DMD_LONGNAME,
        "OpenDRIVE - Open Dynamic Road Information for Vehicle Environment");
    driver->SetMetadataItem(GDAL_DMD_EXTENSION, "xodr");

    driver->pfnOpen = OGRXODRDriverOpen;
    driver->pfnIdentify = OGRXODRDriverIdentity;

    GetGDALDriverManager()->RegisterDriver(driver);
}
