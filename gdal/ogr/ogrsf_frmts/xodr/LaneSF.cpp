/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneSF.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 26, 2019, 4:24 PM
 */

#include "LaneSF.h"

LaneSF::LaneSF (LineString& laneLineString,lane& roadLane):
     roadLane(roadLane)
{
    lineString = static_cast<LineString*>(geometryFactory->createGeometry(&laneLineString));

}

LaneSF::LaneSF (const LaneSF& orig):
lineString(orig.lineString),
        roadLane(orig.roadLane)
{ }

LaneSF::~LaneSF () {
	lineString = NULL;
	delete(lineString);
}

lane
LaneSF::GetRoadLane () const
{
  return roadLane;
}

LineString*
LaneSF::getGeometry()
{
  return lineString ;
}

