/******************************************************************************
 *
 * Project:  GDAL
 * Purpose:  Test OpenFileGDB write support
 * Author:   Even Rouault, <even dot rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2022, Even Rouault <even dot rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "filegdbtable.h"
#include "ogr_api.h"

using namespace OpenFileGDB;

int main()
{
    FileGDBTable oTable;
    FileGDBTableGeometryType eTableGeomType = FGTGT_MULTIPOINT;
    bool bGeomTypeHasZ = true;
    bool bGeomTypeHasM = true;
    const int nTablxOffsetSize = 4;
    oTable.Create("test_ofgdb.gdbtable", nTablxOffsetSize, eTableGeomType,
                  bGeomTypeHasZ, bGeomTypeHasM);
    oTable.CreateField(std::unique_ptr<FileGDBField>(new FileGDBField(
        "OBJECTID", "OBJECTID", FGFT_OBJECTID,
        /* nullable = */ false,
        /* required = */ true,
        /* editable = */ false, 0, FileGDBField::UNSET_FIELD)));

    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("int16", "", FGFT_INT16, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("int32", "", FGFT_INT32, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("float32", "", FGFT_FLOAT32, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("float64", "", FGFT_FLOAT64, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("str", "", FGFT_STRING, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("datetime", "", FGFT_DATETIME, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));
    oTable.CreateField(std::unique_ptr<FileGDBField>(
        new FileGDBField("binary", "", FGFT_BINARY, true, false, true, 0,
                         FileGDBField::UNSET_FIELD)));

    auto poGeomField = std::unique_ptr<FileGDBGeomField>(new FileGDBGeomField(
        "SHAPE", "", true, "{B286C06B-0879-11D2-AACA-00C04FA33C20}", -400, -400,
        1000000000, 0.000000008983153, {0}));
    poGeomField->SetZOriginScaleTolerance(0, 1, 0.001);
    poGeomField->SetMOriginScaleTolerance(0, 1, 0.001);

    oTable.CreateField(std::move(poGeomField));

    std::vector<OGRField> fields(9, FileGDBField::UNSET_FIELD);
    oTable.CreateFeature(fields, nullptr);

    fields[1].Integer = -32768;
    fields[2].Integer = 123456789;
    fields[3].Real = 1.25;
    fields[4].Real = 1.256789012;
    fields[5].String = const_cast<char *>("foo");
    fields[6].Date.Year = 2022;
    fields[6].Date.Month = 4;
    fields[6].Date.Day = 5;
    fields[6].Date.Hour = 12;
    fields[6].Date.Minute = 34;
    fields[6].Date.Second = 56;
    fields[6].Date.TZFlag = 0;
    fields[7].Binary.nCount = 1;
    fields[7].Binary.paData =
        reinterpret_cast<GByte *>(const_cast<char *>("\x01"));
    OGRPoint p(1, 2, 3, 4);
    oTable.CreateFeature(fields, &p);

    OGRMultiPoint mp;
    mp.addGeometryDirectly(new OGRPoint(1, 2, 3, 4));
    mp.addGeometryDirectly(new OGRPoint(5, 0, 6, 7));
    oTable.CreateFeature(fields, &mp);

    OGRLineString ls;
    ls.addPoint(1, 2, 3, 4);
    ls.addPoint(5, 0, 6, 7);
    oTable.CreateFeature(fields, &ls);

    OGRMultiLineString mls;
    mls.addGeometry(&ls);
    mls.addGeometry(&ls);
    mls.addGeometry(&ls);
    oTable.CreateFeature(fields, &mls);

    {
        OGRPolygon poly;
        OGRLinearRing *poLR = new OGRLinearRing();
        poLR->addPoint(0, 0);
        poLR->addPoint(1, 0);
        poLR->addPoint(1, 1);
        poLR->addPoint(0, 1);
        poLR->addPoint(0, 0);
        poly.addRingDirectly(poLR);
        oTable.CreateFeature(fields, &poly);
    }

    {
        OGRPolygon poly;
        OGRLinearRing *poLR = new OGRLinearRing();
        poLR->addPoint(0, 0);
        poLR->addPoint(1, 0);
        poLR->addPoint(1, 1);
        poLR->addPoint(0, 1);
        poLR->addPoint(0, 0);
        poly.addRingDirectly(poLR);
        poLR = new OGRLinearRing();
        poLR->addPoint(0.1, 0.1);
        poLR->addPoint(0.9, 0.1);
        poLR->addPoint(0.9, 0.9);
        poLR->addPoint(0.1, 0.1);
        poly.addRingDirectly(poLR);
        oTable.CreateFeature(fields, &poly);
    }

    {
        OGRMultiPolygon mpoly;
        OGRPolygon *poPoly = new OGRPolygon();
        OGRLinearRing *poLR = new OGRLinearRing();
        poLR->addPoint(0, 0);
        poLR->addPoint(1, 0);
        poLR->addPoint(1, 1);
        poLR->addPoint(0, 1);
        poLR->addPoint(0, 0);
        poPoly->addRingDirectly(poLR);
        mpoly.addGeometryDirectly(poPoly);
        oTable.CreateFeature(fields, &mpoly);
    }

    {
        OGRMultiPolygon mpoly;
        {
            OGRPolygon *poPoly = new OGRPolygon();
            OGRLinearRing *poLR = new OGRLinearRing();
            poLR->addPoint(0, 0);
            poLR->addPoint(1, 0);
            poLR->addPoint(1, 1);
            poLR->addPoint(0, 1);
            poLR->addPoint(0, 0);
            poPoly->addRingDirectly(poLR);
            mpoly.addGeometryDirectly(poPoly);
        }
        {
            OGRPolygon *poPoly = new OGRPolygon();
            OGRLinearRing *poLR = new OGRLinearRing();
            poLR->addPoint(10, 0);
            poLR->addPoint(11, 0);
            poLR->addPoint(11, 1);
            poLR->addPoint(10, 1);
            poLR->addPoint(10, 0);
            poPoly->addRingDirectly(poLR);
            mpoly.addGeometryDirectly(poPoly);
        }
        oTable.CreateFeature(fields, &mpoly);
    }

    return 0;
}
