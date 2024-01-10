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
# Copyright 2023 German Aerospace Center (DLR), Institute of Transportation Systems
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

import os
import shutil
import pytest

from osgeo import gdal, ogr

pytestmark = pytest.mark.require_driver("XODR")
###############################################################################
#!TODO :
# add check for empty geometries when DISSOLVE_TIN == YES
###############################################################################
############################################################################

xodr_data_path = ["data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr"]

################################    TEST-1    ############################################
## Test:
## - Driver availability
## - Data source
## - Layer count 

@pytest.mark.parametrize("xodr_file_path", xodr_data_path)

def test_ogr_xodr_1(xodr_file_path):
    drv = ogr.GetDriverByName("XODR")
    if drv is None:
        pytest.skip("XODR driver not available")
    
    # Data Source test
    ds = ogr.Open(xodr_file_path)
    assert ds is not None, f"Cannot open dataset for file: {xodr_file_path}"

    assert ds.TestCapability("foo") == 0  # TODO: Should we do something in TestCapability?

    assert ds.GetLayerCount() == 6, f"Bad layer count for file: {xodr_file_path}"

################################    TEST-2    ############################################
## Test:
## - DISSOLVE_TIN = NO  
## - EPS = 1.0
## ogr_xodr_check_layer(datasource: ds, layer_index: i, dissolve_tin: False, eps: 1.0): 

test_ogr_xodr_option_2 = ["DISSOLVE_TIN=NO", "EPS=1.0"]

@pytest.mark.parametrize("xodr_file_path, options", [(xodr_data_path[0], test_ogr_xodr_option_2)])

def test_ogr_xodr_2(xodr_file_path, options):
    ds = gdal.OpenEx(
        xodr_file_path,
        gdal.OF_VECTOR,
        open_options=options,
    )
    for i in range(ds.GetLayerCount()):
        ogr_xodr_check_layer(ds, i, False, 1.0)

################################    TEST-3    ############################################
## Test:
## - DISSOLVE_TIN = YES  
## - EPS = 1.0
## ogr_xodr_check_layer(datasource: ds, layer_index: i, dissolve_tin: True, eps: 1.0): 

test_ogr_xodr_option_3 = ["DISSOLVE_TIN=YES", "EPS=1.0"]

@pytest.mark.parametrize("xodr_file_path, options", [(xodr_data_path[0], test_ogr_xodr_option_3)])

def test_ogr_xodr_3(xodr_file_path, options):
    ds = gdal.OpenEx(
        xodr_file_path,
        gdal.OF_VECTOR,
        open_options=options,
    )
    for i in range(ds.GetLayerCount()):
        ogr_xodr_check_layer(ds, i, True, 1.0)

################################    TEST-4    ############################################
## Test:
## - DISSOLVE_TIN = NO  
## - EPS = 0.1
## ogr_xodr_check_layer(datasource: ds, layer_index: i, dissolve_tin: False, eps: 1.0): 

test_ogr_xodr_option_4 = ["DISSOLVE_TIN=NO", "EPS=0.1"]

@pytest.mark.parametrize("xodr_file_path, options", [(xodr_data_path[0], test_ogr_xodr_option_4)])

def test_ogr_xodr_4(xodr_file_path, options):
    """Test geometry output for EPS=0.1
    """ 
    
    ds = gdal.OpenEx(
        xodr_file_path,
        gdal.OF_VECTOR,
        open_options= options,
    )
    ogr_xodr_check_layer(ds, 0, False, 0.1)

################################    TEST-5   ############################################
## Test:
## Check spatial reference system for each layer 

@pytest.mark.parametrize("xodr_file_path", xodr_data_path)

def test_ogr_xodr_5(xodr_file_path):
    ds = ogr.Open(xodr_file_path)
    for i in range(ds.GetLayerCount()):
        lyr = ds.GetLayer(i)
        srs_proj4 = lyr.GetSpatialRef().ExportToProj4()
        expected_proj4 = '+proj=utm +zone=32 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs'
        assert srs_proj4 == expected_proj4, "bad spatial ref" 

##################### ogr_xodr_check_layer ##################################
## Default function to set value for each condition in test cases
## ds : datasource (string)
## lyrInd : layer index (integer)
## dissolve_tin : dissolve triangulated surfaces (boolean)
## eps: value for linear approximation (float)

def ogr_xodr_check_layer(ds, lyrInd, dissolve_tin, eps):
    if lyrInd == 0:
        ## Layer 0 : Reference Line 
        lyr = ds.GetLayer(0)
        assert lyr.GetName() == "ReferenceLine", "bad layer name"
        assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
        assert lyr.GetFeatureCount() == 41
        assert lyr.GetLayerDefn().GetFieldCount() == 3
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTReal
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        )
        
        # Test geometry output for EPS=1.0
        lyr.ResetReading()
        feat = lyr.GetNextFeature()
        if(eps == 1.0):
            assert feat.GetGeometryRef().ExportToWkt() == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"
        # Test geometry output for EPS=0.1
        elif(eps == 0.1):
            assert feat.GetGeometryRef().ExportToWkt() == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618257.937110798 5809506.62607284 102.364759846201,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"
        lyr.ResetReading()

    elif lyrInd == 1: 
        ## Layer 1 : Lane Border
        lyr = ds.GetLayer(1)
        assert lyr.GetName() == "LaneBorder", "bad layer name"
        assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
        assert lyr.GetFeatureCount() == 230
        assert lyr.GetLayerDefn().GetFieldCount() == 5
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTInteger
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTInteger
            and lyr.GetLayerDefn().GetFieldDefn(4).GetType() == ogr.OFTInteger
        )
                
    elif lyrInd == 2:
        ## Layer 2 : Road Mark
        lyr = ds.GetLayer(2)
        assert lyr.GetName() == "RoadMark", "bad layer name"
        if(dissolve_tin == False):
            assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
        else:
            assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
        assert lyr.GetFeatureCount() == 424
        assert lyr.GetLayerDefn().GetFieldCount() == 3
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTInteger
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
        )
    elif lyrInd == 3:
        ## Layer 3 : Road Object 
        lyr = ds.GetLayer(3)
        assert lyr.GetName() == "RoadObject", "bad layer name"
        if(dissolve_tin == False):
            assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option
        else:
            assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"

        assert lyr.GetFeatureCount() == 273
        assert lyr.GetLayerDefn().GetFieldCount() == 4
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
        )
    elif lyrInd == 4:
        ## Layer 4 : Lane
        lyr = ds.GetLayer(4)
        assert lyr.GetName() == "Lane", "bad layer name"
        if(dissolve_tin == False):
            assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
        else:
            assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"

        assert lyr.GetFeatureCount() == 174
        assert lyr.GetLayerDefn().GetFieldCount() == 5
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTInteger
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTInteger
            and lyr.GetLayerDefn().GetFieldDefn(4).GetType() == ogr.OFTInteger
        )
    elif lyrInd == 5:
        ## Layer 5 : Road Signal
        lyr = ds.GetLayer(5)
        assert lyr.GetName() == "RoadSignal", "bad layer name"
        if(dissolve_tin == False):
            assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
        else:
            assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
 
        assert lyr.GetFeatureCount() == 50
        assert lyr.GetLayerDefn().GetFieldCount() == 4
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
        )