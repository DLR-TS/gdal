/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Created:  2017
 * Modified: February 2023
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

void RegisterOGRXODR()
{
    if( GDALGetDriverByName("XODR") != NULL )
        return;

    GDALDriver *poDriver = new GDALDriver();

    poDriver->SetDescription("XODR");
    poDriver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
    poDriver->SetMetadataItem(GDAL_DMD_LONGNAME, "Long name for XODR driver");
    poDriver->SetMetadataItem(GDAL_DMD_EXTENSION, "xodr");

    poDriver->pfnOpen = OGRXODRDriverOpen;
    poDriver->pfnIdentify = OGRXODRDriverIdentify; //TODO read-only
    //poDriver->pfnCreate = OGRXODRDriverCreate;

    poDriver->SetMetadataItem(GDAL_DCAP_VIRTUALIO, "YES");

    GetGDALDriverManager()->RegisterDriver(poDriver);
}

static GDALDataset *OGRXODRDriverOpen( GDALOpenInfo* poOpenInfo )
{
    if( !OGRXODRDriverIdentify(poOpenInfo) )
        return NULL;

    OGRXODRDataSource *poDS = new OGRXODRDataSource();
    if( !poDS->Open(poOpenInfo->pszFilename, poOpenInfo->eAccess == GA_Update) )
    {
        delete poDS;
        return NULL;
    }

    return poDS;
}

static int OGRXODRDriverIdentify( GDALOpenInfo* poOpenInfo )
{
    return EQUAL(CPLGetExtension(poOpenInfo->pszFilename), "xodr");
}