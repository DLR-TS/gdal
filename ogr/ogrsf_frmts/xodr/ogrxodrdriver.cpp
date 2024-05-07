/******************************************************************************
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OGRXODRDriver.
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

#include "ogr_xodr.h"
#include "cpl_conv.h"
#include "cpl_error.h"

extern "C" void CPL_DLL RegisterOGRXODR();

static GDALDataset *OGRXODRDriverOpen(GDALOpenInfo *poOpenInfo)
{
    if (poOpenInfo->eAccess == GA_Update || poOpenInfo->fpL == nullptr)
        return nullptr;

    OGRXODRDataSource *dataSource = new OGRXODRDataSource();

    if (!dataSource->Open(poOpenInfo->pszFilename, poOpenInfo->papszOpenOptions))
    {
        delete dataSource;
        dataSource = nullptr;
    }

    return dataSource;
}

static int OGRXODRDriverIdentity(GDALOpenInfo *poOpenInfo)
{
    return EQUAL(CPLGetExtension(poOpenInfo->pszFilename), "xodr");
}

void RegisterOGRXODR()
{
    if (GDALGetDriverByName("XODR") != nullptr)
        return;

    GDALDriver *poDriver = new GDALDriver();
    poDriver->SetDescription("XODR");
    poDriver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
    poDriver->SetMetadataItem(
        GDAL_DMD_LONGNAME,
        "OpenDRIVE - Open Dynamic Road Information for Vehicle Environment");
    poDriver->SetMetadataItem(GDAL_DMD_EXTENSION, "xodr");
    poDriver->SetMetadataItem(
        GDAL_DMD_OPENOPTIONLIST,
        "<OpenOptionList>"
        "  <Option name='EPSILON' type='float' description='Epsilon value for "
        "linear approximation of continuous OpenDRIVE geometries.' "
        "default='1.0'/>"
        "  <Option name='DISSOLVE_TIN' type='boolean' description='Whether to "
        "dissolve triangulated surfaces.' default= 'NO'/>"
        "</OpenOptionList>");
    poDriver->pfnOpen = OGRXODRDriverOpen;
    poDriver->pfnIdentify = OGRXODRDriverIdentity;

    GetGDALDriverManager()->RegisterDriver(poDriver);
}
