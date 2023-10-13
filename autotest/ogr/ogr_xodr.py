#!/usr/bin/env pytest
# -*- coding: utf-8 -*-
###############################################################################
# $Id$
#
# Project:  OpenGIS Simple Features for OpenDRIVE
# Purpose:  Implementation of OGRXODRDataSource.
# Author:   Michael Scholz, German Aerospace Center (DLR)
#           Gülsen Bardak, German Aerospace Center (DLR)        
#
###############################################################################
# Copyright 2023 German Aerospace Center (DLR), Institute of Transportation Systems

import os
import shutil

#import gdaltest
import pytest

from osgeo import gdal, ogr

pytestmark = pytest.mark.require_driver("XODR")
###############################################################################
#!TODO :
# add GetNextFeature
# check GetSpatialRef 
# add eps, count doesn't change, geometry check method? 
###############################################################################
# Basic tests

def test_ogr_xodr_1():
              drv = ogr.GetDriverByName("XODR")
              if drv is None:
                            pytest.skip()
              
              assert drv.TestCapability("foo") == 0

              ## Data Source test 
              ds = ogr.Open("data/xodr/Town07.xodr")
              assert ds is not None, "cannot open dataset"

              assert ds.TestCapability("foo") == 0 #!TODO  should we do something in TestCapability? 

              assert ds.GetLayerCount() == 6, "bad layer count" #Signal will be optional, how can we implement this part in this case?

def test_ogr_xodr_2():
              ds = gdal.OpenEx(
                            "data/xodr/Town07.xodr",
                            gdal.OF_VECTOR,
                            open_options=["TIN=YES"],
              )
              for i in range(ds.GetLayerCount()):
                            ogr_xodr_check_layer(ds, i, True)
def test_ogr_xodr_3():
              ds = gdal.OpenEx(
                            "data/xodr/Town07.xodr",
                            gdal.OF_VECTOR,
                            open_options=["TIN=NO"],
              )
              for i in range(ds.GetLayerCount()):
                            ogr_xodr_check_layer(ds, i, False)


def ogr_xodr_check_layer(ds, lyrInd, tin_option):
              if lyrInd == 0:
                            ## Layer 0 : Reference Line 
                            lyr = ds.GetLayer(0)
                            assert lyr.GetName() == "ReferenceLine", "bad layer name"
                            assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref"
                            assert lyr.GetFeatureCount() == 277
                            assert lyr.GetLayerDefn().GetFieldCount() == 3
                            assert (
                                          lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
                                          and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTReal
                                          and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
                            )

              elif lyrInd == 1: 
                            ## Layer 1 : Lane Border
                            lyr = ds.GetLayer(1)
                            assert lyr.GetName() == "LaneBorder", "bad layer name"
                            assert lyr.GetGeomType() == ogr.wkbLineString, "bad layer geometry type"            
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref"
                            assert lyr.GetFeatureCount() == 992
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
                            if(tin_option == True):
                                          assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
                            else:
                                          assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
                            assert lyr.GetFeatureCount() == 2625
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
                            if(tin_option == True):
                                          assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
                            else:
                                          assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
                            assert lyr.GetFeatureCount() == 128
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
                            if(tin_option == True):
                                          assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
                            else:
                                          assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
                            assert lyr.GetFeatureCount() == 713
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
                            if(tin_option == True):
                                          assert lyr.GetGeomType() == ogr.wkbTINZ, "bad layer geometry type" #It uses default for layer create option       
                            else:
                                          assert lyr.GetGeomType() == ogr.wkbPolygon, "bad layer geometry type"
                            #assert lyr.GetSpatialRef() is None, "bad spatial ref" 
                            assert lyr.GetFeatureCount() == 56
                            assert lyr.GetLayerDefn().GetFieldCount() == 4
                            assert (
                                          lyr.GetLayerDefn().GetFieldDefn(0).GetType() == ogr.OFTString
                                          and lyr.GetLayerDefn().GetFieldDefn(1).GetType() == ogr.OFTString
                                          and lyr.GetLayerDefn().GetFieldDefn(2).GetType() == ogr.OFTString
                                          and lyr.GetLayerDefn().GetFieldDefn(3).GetType() == ogr.OFTString
                            )
