/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   roadsf.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 15, 2019, 3:25 PM
 */

#ifndef ROADSF_H
#define ROADSF_H

#include "OpenDRIVE_1.4H.h"
#include "planviewcalculator.h"
#include "objectsf.h"
#include "objectprocessor.h"
#include "signalsf.h"
#include "signalprocessor.h"
#include "LanesSF.h"
#include "LaneSF.h"
#include "geos/geom.h"
#include "geos_c.h"
#include "cpl_error.h"

using namespace xml_schema;
using namespace geos::geom;


class RoadSF {
public:
    RoadSF(const OpenDRIVE::road_type& r);
    RoadSF(const RoadSF& orig);
    virtual ~RoadSF();
    geos::geom::LineString* getPlanViewSF();
    OpenDRIVE::road_type getRoad();
    std::vector<LaneSF> getLanesSf();
    std::vector<CenterLaneSF*> getCenterLaneSf();
    std::vector<ObjectSF> getObjectsSf();
    std::vector<SignalSF> getSignalSf();
    
    
private:
    const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
    OpenDRIVE::road_type od_road;
    PlanViewCalculator pvc;
    ObjectProcessor objProc;
    SignalProcessor sigProc;
    LanesSF lanes;
};

#endif /* ROADSF_H */

