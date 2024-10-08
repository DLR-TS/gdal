/******************************************************************************
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements Open FileGDB OGR driver.
 * Author:   Even Rouault, <even dot rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2021, Even Rouault <even dot rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef FILEGDB_GDBTOOGRFIELDTYPE_H
#define FILEGDB_GDBTOOGRFIELDTYPE_H

#include <string>
#include "ogr_api.h"

/*************************************************************************/
/*                            GDBToOGRFieldType()                        */
/*************************************************************************/

// We could make this function far more robust by doing automatic coercion of
// types, and/or skipping fields we do not know. But, for our purposes. this
// works fine.
static bool GDBToOGRFieldType(const std::string &gdbType, OGRFieldType *pOut,
                              OGRFieldSubType *pSubType)
{
    /*
    ESRI types
    esriFieldTypeSmallInteger = 0,
    esriFieldTypeInteger = 1,
    esriFieldTypeSingle = 2,
    esriFieldTypeDouble = 3,
    esriFieldTypeString = 4,
    esriFieldTypeDate = 5,
    esriFieldTypeOID = 6,
    esriFieldTypeGeometry = 7,
    esriFieldTypeBlob = 8,
    esriFieldTypeRaster = 9,
    esriFieldTypeGUID = 10,
    esriFieldTypeGlobalID = 11,
    esriFieldTypeXML = 12
    */

    // OGR Types

    //            Desc                                 Name GDB->OGR Mapped By
    //            Us?
    /** Simple 32bit integer */    //                   OFTInteger = 0, YES
    /** List of 32bit integers */  //                 OFTIntegerList = 1, NO
    /** Double Precision floating point */  //        OFTReal = 2, YES
    /** List of doubles */        //                        OFTRealList = 3, NO
    /** String of ASCII chars */  //                  OFTString = 4, YES
    /** Array of strings */       //                       OFTStringList = 5, NO
    /** deprecated */  //                             OFTWideString = 6, NO
    /** deprecated */  //                             OFTWideStringList = 7, NO
    /** Raw Binary data */  //                        OFTBinary = 8, YES
    /** Date */             //                                   OFTDate = 9, NO
    /** Time */           //                                   OFTTime = 10, NO
    /** Date and Time */  //                          OFTDateTime = 11 YES

    *pSubType = OFSTNone;
    if (gdbType == "esriFieldTypeSmallInteger")
    {
        *pSubType = OFSTInt16;
        *pOut = OFTInteger;
        return true;
    }
    else if (gdbType == "esriFieldTypeInteger")
    {
        *pOut = OFTInteger;
        return true;
    }
    else if (gdbType == "esriFieldTypeSingle")
    {
        *pSubType = OFSTFloat32;
        *pOut = OFTReal;
        return true;
    }
    else if (gdbType == "esriFieldTypeDouble")
    {
        *pOut = OFTReal;
        return true;
    }
    else if (gdbType == "esriFieldTypeGUID" ||
             gdbType == "esriFieldTypeGlobalID" ||
             gdbType == "esriFieldTypeXML" || gdbType == "esriFieldTypeString")
    {
        *pOut = OFTString;
        return true;
    }
    else if (gdbType == "esriFieldTypeDate")
    {
        *pOut = OFTDateTime;
        return true;
    }
    else if (gdbType == "esriFieldTypeBlob")
    {
        *pOut = OFTBinary;
        return true;
    }
    else
    {
        /* Intentionally fail at these
        esriFieldTypeOID
        esriFieldTypeGeometry
        esriFieldTypeRaster
        */
        CPLError(CE_Warning, CPLE_AppDefined, "%s",
                 ("Cannot map field " + gdbType).c_str());

        return false;
    }
}

#endif  // FILEGDB_GDBTOOGRFIELDTYPE_H
