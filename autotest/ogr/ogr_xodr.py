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
# check GetSpatialRef 
# add check for empty geometries when DISSOLVE_TIN == YES
###############################################################################
# Basic tests

def test_ogr_xodr_1():
    drv = ogr.GetDriverByName("XODR")
    if drv is None:
        pytest.skip()
    
    ## Data Source test 
    ds = ogr.Open("data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr")
    assert ds is not None, "cannot open dataset"

    assert ds.TestCapability("foo") == 0 #TODO  should we do something in TestCapability? 

    assert ds.GetLayerCount() == 6, "bad layer count"


def test_ogr_xodr_2():
    ds = gdal.OpenEx(
        "data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr",
        gdal.OF_VECTOR,
        open_options=["DISSOLVE_TIN=NO", "EPS=1.0"],
    )
    for i in range(ds.GetLayerCount()):
        ogr_xodr_check_layer(ds, i, False)


def test_ogr_xodr_3():
    ds = gdal.OpenEx(
        "data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr",
        gdal.OF_VECTOR,
        open_options=["DISSOLVE_TIN=YES", "EPS=1.0"],
    )
    for i in range(ds.GetLayerCount()):
        ogr_xodr_check_layer(ds, i, True)


def ogr_xodr_check_layer(ds, lyrInd, dissolve_tin):
    if lyrInd == 0:
        ## Layer 0 : Reference Line 
        lyr = ds.GetLayer(0)
        assert lyr.GetName() == "ReferenceLine", "bad layer name"
        assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
        #assert lyr.GetSpatialRef() is None, "bad spatial ref"
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
        assert feat.GetGeometryRef().ExportToWkt() == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"
        lyr.ResetReading()

    elif lyrInd == 1: 
        ## Layer 1 : Lane Border
        lyr = ds.GetLayer(1)
        assert lyr.GetName() == "LaneBorder", "bad layer name"
        assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
        #assert lyr.GetSpatialRef() is None, "bad spatial ref"
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
        #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
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
        #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
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
        #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
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
        #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
        assert lyr.GetFeatureCount() == 50
        assert lyr.GetLayerDefn().GetFieldCount() == 4
        assert (
            lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
            and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
        )


def test_ogr_xodr_4():
    """Test geometry output for EPS=0.1
    """ 
    
    ds = gdal.OpenEx(
        "data/xodr/5g_living_lab_A39_Wolfsburg-West.xodr",
        gdal.OF_VECTOR,
        open_options=["DISSOLVE_TIN=NO", "EPS=0.1"],
    )
    lyr = ds.GetLayerByName("ReferenceLine")
    lyr.ResetReading()
    feat = lyr.GetNextFeature()
    assert feat.GetGeometryRef().ExportToWkt() == "LINESTRING (618251.572934302 5809506.96459625 102.378603962182,618254.944363001 5809506.95481165 102.371268481462,618257.937110798 5809506.62607284 102.364759846201,618258.290734177 5809506.56065761 102.363999939623)", "wrong geometry created"
    lyr.ResetReading()
