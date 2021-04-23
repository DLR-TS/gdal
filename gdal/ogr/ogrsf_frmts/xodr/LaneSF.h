/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneSF.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 26, 2019, 4:24 PM
 */

#ifndef LANESF_H
#define LANESF_H

#include "OpenDRIVE_1.4H.h"
#include <geos/geom.h>

using namespace geos::geom;

class LaneSF
{
public:

    LaneSF (LineString& laneLineString,lane& roadLane);
    LaneSF (const LaneSF& orig);
    virtual ~LaneSF ();
    lane GetRoadLane () const;
    LineString* getGeometry();

private:
    const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
    LineString* lineString;
    lane roadLane;

};

#endif /* LANESF_H */

