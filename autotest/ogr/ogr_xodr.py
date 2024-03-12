#!/usr/bin/env pytest
# -*- coding: utf-8 -*-
###############################################################################
#
# Project:  GDAL/OGR Test Suite
# Purpose:  XODR driver testing.
# Author:   Michael Scholz, German Aerospace Center (DLR)
#           Gülsen Bardak, German Aerospace Center (DLR)
#
###############################################################################
# Copyright 2024 German Aerospace Center (DLR), Institute of Transportation Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################
import pytest

from osgeo import gdal, ogr

pytestmark = pytest.mark.require_driver("XODR")
xodr_file = "data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr"


def test_ogr_xodr_basics():
    """Test basic capabilities:
        - Data source
        - Layer count
    """
    ds = gdal.OpenEx(xodr_file, gdal.OF_VECTOR)
    assert ds is not None, f"Cannot open dataset for file: {xodr_file}"
    assert ds.GetLayerCount() == 6, f"Bad layer count for file: {xodr_file}"


def test_ogr_xodr_simple_layers():
    """Test all point and linestring layers for:
        - Correct feature type definitions
        - Spatial reference system
    """
    ds = gdal.OpenEx(xodr_file, gdal.OF_VECTOR)

    layer_reference_line = ds.GetLayer("ReferenceLine")
    check_feat_def_reference_line(layer_reference_line)
    check_spatial_ref(layer_reference_line)

    layer_lane_border = ds.GetLayer("LaneBorder")
    check_feat_def_lane_border(layer_lane_border)
    check_spatial_ref(layer_lane_border)

    layer_road_object = ds.GetLayer("RoadObject")
    check_feat_def_road_object(layer_road_object)
    check_spatial_ref(layer_road_object)

    layer_road_signal = ds.GetLayer("RoadSignal")
    check_feat_def_road_signal(layer_road_signal)
    check_spatial_ref(layer_road_signal)


@pytest.mark.parametrize("dissolve_tin", [True, False])
def test_ogr_xodr_tin_layers_with_dissolve(dissolve_tin: bool):
    """Test all TIN layers for:
        - Correct feature type definitions
        - Spatial reference system

    Args:
        dissolve_tin (bool): True if to dissolve triangulated surfaces.
    """
    options = ["DISSOLVE_TIN=" + str(dissolve_tin)]
    ds = gdal.OpenEx(xodr_file, gdal.OF_VECTOR, open_options=options)

    layer_road_mark = ds.GetLayer("RoadMark")
    check_feat_def_road_mark(layer_road_mark, dissolve_tin)
    check_spatial_ref(layer_road_mark)

    layer_lane = ds.GetLayer("Lane")
    check_feat_def_lane(layer_lane, dissolve_tin)
    check_spatial_ref(layer_lane)


def check_feat_def_reference_line(layer):
    assert layer.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"
    assert layer.GetFeatureCount() == 41
    assert layer.GetLayerDefn().GetFieldCount() == 3
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTReal
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
    )


def check_feat_def_lane_border(layer):
    assert layer.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"
    assert layer.GetFeatureCount() == 230
    assert layer.GetLayerDefn().GetFieldCount() == 5
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTInteger
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTInteger
        and layer.GetLayerDefn().GetFieldDefn(4).GetType() == ogr.OFTInteger
    )


def check_feat_def_road_mark(layer, dissolve_tin: bool):
    if not dissolve_tin:
        assert layer.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type"
    else:
        assert layer.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
    assert layer.GetFeatureCount() == 424
    assert layer.GetLayerDefn().GetFieldCount() == 3
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTInteger
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
    )


def check_feat_def_road_object(layer):
    assert layer.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type"
    assert layer.GetFeatureCount() == 273
    assert layer.GetLayerDefn().GetFieldCount() == 4
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
    )


def check_feat_def_lane(layer, dissolve_tin: bool):
    if not dissolve_tin:
        assert layer.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type"
    else:
        assert layer.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
    assert layer.GetFeatureCount() == 174
    assert layer.GetLayerDefn().GetFieldCount() == 5
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTInteger
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTInteger
        and layer.GetLayerDefn().GetFieldDefn(4).GetType() == ogr.OFTInteger
    )


def check_feat_def_road_signal(layer):
    assert layer.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type"
    assert layer.GetFeatureCount() == 50
    assert layer.GetLayerDefn().GetFieldCount() == 4
    assert (
        layer.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        and layer.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
    )


def check_spatial_ref(layer):
    srs_proj4 = layer.GetSpatialRef().ExportToProj4()
    expected_proj4 = '+proj=utm +zone=32 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs'
    assert srs_proj4 == expected_proj4, "bad spatial ref"


@pytest.mark.parametrize("eps", [1.0, 0.1])
def test_ogr_xodr_geometry_eps(eps: float):
    """Test correct geometry creation for different values of open option EPS.

    Args:
        eps (float): Value for linear approximation of parametric geometries.
    """
    options = ["EPS=" + str(eps)]
    ds = gdal.OpenEx(xodr_file, gdal.OF_VECTOR, open_options=options)

    lyr = ds.GetLayer("ReferenceLine")
    ogr_xodr_check_reference_line_geometry_eps(lyr, eps)


def ogr_xodr_check_reference_line_geometry_eps(lyr, eps: float):
    lyr.ResetReading()
    feat = lyr.GetNextFeature()
    wkt = feat.GetGeometryRef().ExportToWkt()
    if eps == 1.0:
        assert wkt == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"
    elif eps == 0.1:
        assert wkt == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618257.937110798 5809506.62607284 102.364759846201,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"


@pytest.mark.parametrize("dissolve_tin", [True, False])
def test_ogr_xodr_geometry_dissolve(dissolve_tin: bool):
    """Test correct geometry creation for different values of open option DISSOLVE_TIN.

    Args:
        dissolve_tin (bool): True if to dissolve triangulated surfaces.
    """
    options = ["DISSOLVE_TIN=" + str(dissolve_tin)]
    ds = gdal.OpenEx(xodr_file, gdal.OF_VECTOR, open_options=options)

    lyr = ds.GetLayer("Lane")
    ogr_xodr_check_lane_geometry_dissolve(lyr, dissolve_tin)


def ogr_xodr_check_lane_geometry_dissolve(lyr, dissolve_tin: bool):
    lyr.ResetReading()
    feat = lyr.GetNextFeature()
    wkt = feat.GetGeometryRef().ExportToWkt()
    if not dissolve_tin:
        assert wkt == "TIN Z (((618251.708293914 5809503.30115552 102.206436434521,618253.406110685 5809502.59383908 102.162274831603,618253.40871869 5809503.08668632 102.186041767762,618251.708293914 5809503.30115552 102.206436434521)),((618251.708293914 5809503.30115552 102.206436434521,618251.726901715 5809502.7975446 102.182768671482,618253.406110685 5809502.59383908 102.162274831603,618251.708293914 5809503.30115552 102.206436434521)),((618253.40871869 5809503.08668632 102.186041767762,618254.710111278 5809502.39980074 102.146632509166,618254.735144074 5809502.88656198 102.170637739305,618253.40871869 5809503.08668632 102.186041767762)),((618253.40871869 5809503.08668632 102.186041767762,618253.406110685 5809502.59383908 102.162274831603,618254.710111278 5809502.39980074 102.146632509166,618253.40871869 5809503.08668632 102.186041767762)),((618254.735144074 5809502.88656198 102.170637739305,618256.354637481 5809502.1051039 102.128452978327,618256.414547031 5809502.56472816 102.151918900654,618254.735144074 5809502.88656198 102.170637739305)),((618254.735144074 5809502.88656198 102.170637739305,618254.710111278 5809502.39980074 102.146632509166,618256.354637481 5809502.1051039 102.128452978327,618254.735144074 5809502.88656198 102.170637739305)),((618256.414547031 5809502.56472816 102.151918900654,618257.381896193 5809501.87667676 102.118091279345,618257.465586929 5809502.30800315 102.140735883984,618256.414547031 5809502.56472816 102.151918900654)),((618256.414547031 5809502.56472816 102.151918900654,618256.354637481 5809502.1051039 102.128452978327,618257.381896193 5809501.87667676 102.118091279345,618256.414547031 5809502.56472816 102.151918900654)))", "wrong geometry created"
    else:
        assert wkt == "POLYGON ((618257.381896193 5809501.87667676 102.118091279345,618256.354637481 5809502.1051039 102.128452978327,618254.710111278 5809502.39980074 102.146632509166,618253.406110685 5809502.59383908 102.162274831603,618251.726901715 5809502.7975446 102.182768671482,618251.708293914 5809503.30115552 102.206436434521,618253.40871869 5809503.08668632 102.186041767762,618254.735144074 5809502.88656198 102.170637739305,618256.414547031 5809502.56472816 102.151918900654,618257.465586929 5809502.30800315 102.140735883984,618257.381896193 5809501.87667676 102.118091279345))", "wrong geometry created"
