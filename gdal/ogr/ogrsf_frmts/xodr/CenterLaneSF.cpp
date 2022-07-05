/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CenterLaneSF.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 26, 2019, 11:39 AM
 */

#include "CenterLaneSF.h"
CenterLaneSF::CenterLaneSF()
{
	this->lineString = NULL;
}

CenterLaneSF::CenterLaneSF (const CoordinateArraySequence& laneLineString)
{	
	
	this->lineString = geometryFactory->createLineString(laneLineString);

}

CenterLaneSF::CenterLaneSF (const CenterLaneSF& orig):
	lineString(orig.lineString)
{
    
}

CenterLaneSF::~CenterLaneSF () {
	delete(this->lineString);
}

LineString* CenterLaneSF::getGeometry()
{
  return lineString;
}

int CenterLaneSF::getID(){
    return 0;
}
