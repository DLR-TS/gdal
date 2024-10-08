/******************************************************************************
 *
 * Project:  MVT Translator
 * Purpose:  Mapbox Vector Tile decoder
 * Author:   Even Rouault, Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "mvtutils.h"
#include "ogr_api.h"

/************************************************************************/
/*                        OGRMVTInitFields()                            */
/************************************************************************/

void OGRMVTInitFields(OGRFeatureDefn *poFeatureDefn,
                      const CPLJSONObject &oFields,
                      const CPLJSONArray &oAttributesFromTileStats)
{
    {
        OGRFieldDefn oFieldDefnId("mvt_id", OFTInteger64);
        poFeatureDefn->AddFieldDefn(&oFieldDefnId);
    }

    if (oFields.IsValid())
    {
        for (const auto &oField : oFields.GetChildren())
        {
            if (oField.GetType() == CPLJSONObject::Type::String)
            {
                if (oField.ToString() == "Number")
                {
                    OGRFieldDefn oFieldDefn(oField.GetName().c_str(), OFTReal);

                    for (int i = 0; i < oAttributesFromTileStats.Size(); ++i)
                    {
                        if (oAttributesFromTileStats[i].GetString(
                                "attribute") == oField.GetName() &&
                            oAttributesFromTileStats[i].GetString("type") ==
                                "number")
                        {
                            const auto eMinType = oAttributesFromTileStats[i]
                                                      .GetObj("min")
                                                      .GetType();
                            const auto eMaxType = oAttributesFromTileStats[i]
                                                      .GetObj("max")
                                                      .GetType();
                            if (eMinType == CPLJSONObject::Type::Integer &&
                                eMaxType == CPLJSONObject::Type::Integer)
                            {
                                oFieldDefn.SetType(OFTInteger);
                            }
                            else if ((eMinType ==
                                          CPLJSONObject::Type::Integer ||
                                      eMinType == CPLJSONObject::Type::Long) &&
                                     eMaxType == CPLJSONObject::Type::Long)
                            {
                                oFieldDefn.SetType(OFTInteger64);
                            }
                            break;
                        }
                    }

                    poFeatureDefn->AddFieldDefn(&oFieldDefn);
                }
                else if (oField.ToString() == "Integer")  // GDAL extension
                {
                    OGRFieldDefn oFieldDefn(oField.GetName().c_str(),
                                            OFTInteger);
                    poFeatureDefn->AddFieldDefn(&oFieldDefn);
                }
                else if (oField.ToString() == "Boolean")
                {
                    OGRFieldDefn oFieldDefn(oField.GetName().c_str(),
                                            OFTInteger);
                    oFieldDefn.SetSubType(OFSTBoolean);
                    poFeatureDefn->AddFieldDefn(&oFieldDefn);
                }
                else
                {
                    OGRFieldDefn oFieldDefn(oField.GetName().c_str(),
                                            OFTString);
                    poFeatureDefn->AddFieldDefn(&oFieldDefn);
                }
            }
        }
    }
}

/************************************************************************/
/*                     OGRMVTFindGeomTypeFromTileStat()                 */
/************************************************************************/

OGRwkbGeometryType
OGRMVTFindGeomTypeFromTileStat(const CPLJSONArray &oTileStatLayers,
                               const char *pszLayerName)
{
    OGRwkbGeometryType eGeomType = wkbUnknown;
    for (int i = 0; i < oTileStatLayers.Size(); i++)
    {
        CPLJSONObject oId = oTileStatLayers[i].GetObj("layer");
        if (oId.IsValid() && oId.GetType() == CPLJSONObject::Type::String)
        {
            if (oId.ToString() == pszLayerName)
            {
                CPLJSONObject oGeom = oTileStatLayers[i].GetObj("geometry");
                if (oGeom.IsValid() &&
                    oGeom.GetType() == CPLJSONObject::Type::String)
                {
                    const std::string oGeomType(oGeom.ToString());
                    // Note: this information is not
                    // reliable in case
                    // of mix of geometry types
                    if (oGeomType == "Point")
                    {
                        eGeomType = wkbMultiPoint;
                    }
                    else if (oGeomType == "LineString")
                    {
                        eGeomType = wkbMultiLineString;
                    }
                    else if (oGeomType == "Polygon")
                    {
                        eGeomType = wkbMultiPolygon;
                    }
                }
                break;
            }
        }
    }
    return eGeomType;
}

/************************************************************************/
/*                     OGRMVTFindAttributesFromTileStat()               */
/************************************************************************/

CPLJSONArray
OGRMVTFindAttributesFromTileStat(const CPLJSONArray &oTileStatLayers,
                                 const char *pszLayerName)
{
    for (int i = 0; i < oTileStatLayers.Size(); i++)
    {
        CPLJSONObject oId = oTileStatLayers[i].GetObj("layer");
        if (oId.IsValid() && oId.GetType() == CPLJSONObject::Type::String)
        {
            if (oId.ToString() == pszLayerName)
            {
                CPLJSONObject oAttributes =
                    oTileStatLayers[i].GetObj("attributes");
                if (oAttributes.IsValid() &&
                    oAttributes.GetType() == CPLJSONObject::Type::Array)
                {
                    return oAttributes.ToArray();
                }
                break;
            }
        }
    }
    CPLJSONArray oAttributes;
    oAttributes.Deinit();
    return oAttributes;
}

/************************************************************************/
/*                     OGRMVTCreateFeatureFrom()                        */
/************************************************************************/

OGRFeature *OGRMVTCreateFeatureFrom(OGRFeature *poSrcFeature,
                                    OGRFeatureDefn *poTargetFeatureDefn,
                                    bool bJsonField, OGRSpatialReference *poSRS)
{
    OGRFeature *poFeature = new OGRFeature(poTargetFeatureDefn);
    if (bJsonField)
    {
        CPLJSONObject oProperties;
        bool bEmpty = true;
        for (int i = 1; i < poSrcFeature->GetFieldCount(); i++)
        {
            if (poSrcFeature->IsFieldSet(i))
            {
                bEmpty = false;
                OGRFieldDefn *poFDefn = poSrcFeature->GetFieldDefnRef(i);
                if (poSrcFeature->IsFieldNull(i))
                {
                    oProperties.AddNull(poFDefn->GetNameRef());
                }
                else if (poFDefn->GetType() == OFTInteger ||
                         poFDefn->GetType() == OFTInteger64)
                {
                    if (poFDefn->GetSubType() == OFSTBoolean)
                    {
                        oProperties.Add(poFDefn->GetNameRef(),
                                        poSrcFeature->GetFieldAsInteger(i) ==
                                            1);
                    }
                    else
                    {
                        oProperties.Add(poFDefn->GetNameRef(),
                                        poSrcFeature->GetFieldAsInteger64(i));
                    }
                }
                else if (poFDefn->GetType() == OFTReal)
                {
                    oProperties.Add(poFDefn->GetNameRef(),
                                    poSrcFeature->GetFieldAsDouble(i));
                }
                else
                {
                    oProperties.Add(poFDefn->GetNameRef(),
                                    poSrcFeature->GetFieldAsString(i));
                }
            }
        }
        if (!bEmpty)
        {
            poFeature->SetField(
                "json", oProperties.Format(CPLJSONObject::PrettyFormat::Pretty)
                            .c_str());
        }

        OGRGeometry *poSrcGeom = poSrcFeature->GetGeometryRef();
        if (poSrcGeom)
        {
            poFeature->SetGeometry(poSrcGeom);
        }
#ifdef nodef
        CPLJSONObject oObj;
        oObj.Add("type", "Feature");
        if (poSrcFeature->IsFieldSet(0))
            oObj.Add("id", poSrcFeature->GetFieldAsInteger64("mvt_id"));
        oObj.Add("properties", oProperties);
        if (poSrcGeom)
        {
            char *pszGeomJson =
                OGR_G_ExportToJson(OGRGeometry::ToHandle(poSrcGeom));
            CPLJSONDocument oJSonDoc;
            oJSonDoc.LoadMemory(reinterpret_cast<const GByte *>(pszGeomJson));
            CPLFree(pszGeomJson);
            oObj.Add("geometry", oJSonDoc.GetRoot());
        }
        poFeature->SetNativeData(
            oObj.Format(CPLJSONObject::PrettyFormat::Pretty).c_str());
        poFeature->SetNativeMediaType("application/vnd.geo+json");
#endif
    }
    else
    {
        poFeature->SetFrom(poSrcFeature);
    }
    OGRGeometry *poGeom = poFeature->GetGeometryRef();
    if (poGeom)
        poGeom->assignSpatialReference(poSRS);
    return poFeature;
}
