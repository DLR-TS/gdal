/******************************************************************************
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements Open FileGDB OGR driver.
 * Author:   Even Rouault, <even dot rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2024, Even Rouault <even dot rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef FILEGDB_COORDPREC_READ_H
#define FILEGDB_COORDPREC_READ_H

#include "cpl_minixml.h"

#include "ogr_geomcoordinateprecision.h"

/*************************************************************************/
/*                      GDBGridSettingsToOGR()                           */
/*************************************************************************/

/** Fill OGRGeomCoordinatePrecision from XML "psSpatialReference" node.
 */
static OGRGeomCoordinatePrecision
GDBGridSettingsToOGR(const CPLXMLNode *psSpatialReference)
{
    OGRGeomCoordinatePrecision oCoordPrec;
    const char *pszXYScale =
        CPLGetXMLValue(psSpatialReference, "XYScale", nullptr);
    if (pszXYScale && CPLAtof(pszXYScale) > 0)
    {
        oCoordPrec.dfXYResolution = 1.0 / CPLAtof(pszXYScale);
    }
    const char *pszZScale =
        CPLGetXMLValue(psSpatialReference, "ZScale", nullptr);
    if (pszZScale && CPLAtof(pszZScale) > 0)
    {
        oCoordPrec.dfZResolution = 1.0 / CPLAtof(pszZScale);
    }
    const char *pszMScale =
        CPLGetXMLValue(psSpatialReference, "MScale", nullptr);
    if (pszMScale && CPLAtof(pszMScale) > 0)
    {
        oCoordPrec.dfMResolution = 1.0 / CPLAtof(pszMScale);
    }

    CPLStringList aosSpecificOptions;
    for (const CPLXMLNode *psChild = psSpatialReference->psChild; psChild;
         psChild = psChild->psNext)
    {
        if (psChild->eType == CXT_Element &&
            // The 3 below values are only generated by the FileGDB SDK
            !EQUAL(psChild->pszValue, "WKID") &&
            !EQUAL(psChild->pszValue, "LatestWKID") &&
            !EQUAL(psChild->pszValue, "WKT"))
        {
            const char *pszValue = CPLGetXMLValue(psChild, "", "");
            if (CPLGetValueType(pszValue) == CPL_VALUE_REAL)
                pszValue = CPLSPrintf("%.15g", CPLAtof(pszValue));
            aosSpecificOptions.SetNameValue(psChild->pszValue, pszValue);
        }
    }
    oCoordPrec.oFormatSpecificOptions["FileGeodatabase"] =
        std::move(aosSpecificOptions);
    return oCoordPrec;
}

#endif /* FILEGDB_COORDPREC_READ_H */
