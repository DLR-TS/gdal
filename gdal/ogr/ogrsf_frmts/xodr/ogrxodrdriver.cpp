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
