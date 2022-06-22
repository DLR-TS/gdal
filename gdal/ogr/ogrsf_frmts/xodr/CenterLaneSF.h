/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CenterLaneSF.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 26, 2019, 11:39 AM
 */

#ifndef CENTERLANESF_H
#define CENTERLANESF_H

#include <geos/geom.h>
#include <geos_c.h>
#include "OpenDRIVE_1.4H.h"

using namespace geos::geom;

class CenterLaneSF
{
public:
    CenterLaneSF();
    CenterLaneSF (const CoordinateArraySequence& laneLineString);
    CenterLaneSF (const CenterLaneSF& orig);
    virtual ~CenterLaneSF ();
    int getID();
    LineString* getGeometry();

private:
	const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
	LineString* lineString;
};

#endif /* CENTERLANESF_H */

