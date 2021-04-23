/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   roadsf.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 15, 2019, 3:25 PM
 */

#include "roadsf.h"

RoadSF::RoadSF(const OpenDRIVE::road_type& r):
        od_road(r),
        pvc(PlanViewCalculator(this->od_road.planView())),
        lanes(LanesSF(this->pvc, &this->od_road.lanes(), this->od_road.length().get()))
{
    
    if(this->od_road.objects() != NULL){
        this->objProc = ObjectProcessor(this->od_road.objects().get(), this->pvc);   
    }
    else{
        this->objProc = ObjectProcessor();
    }
    if(this->od_road.signals() != NULL){
        this->sigProc = SignalProcessor(this->od_road.signals().get(), this->pvc);
    }
    else{
        this->sigProc = SignalProcessor();
    }
    
}

RoadSF::RoadSF(const RoadSF& orig):
    od_road(orig.od_road),
    pvc(orig.pvc),
    lanes(LanesSF(orig.pvc, &od_road.lanes (), od_road.length ().get ()))
{
    if(this->od_road.objects() != NULL){
        this->objProc = ObjectProcessor(this->od_road.objects().get(), this->pvc);
    }
    else{
        this->objProc = ObjectProcessor();
    }

    if(this->od_road.signals() != NULL){
        this->sigProc = SignalProcessor(this->od_road.signals().get(), this->pvc);
    }
    else{
        this->sigProc = SignalProcessor();
    }
}
RoadSF::~RoadSF()
{
	
}
geos::geom::LineString* RoadSF::getPlanViewSF() {
    CoordinateArraySequence seq;
    double stepSize = od_road.length().get() / 1000;
    for(double s  = 0.0;s < od_road.length().get();s += stepSize)
    {
        seq.add(pvc.getPlanViewPoint(s));
    }
    seq.add(pvc.getPlanViewPoint(od_road.length().get()));
    geos::geom::LineString *l = geometryFactory->createLineString(seq);
    return l;
}

std::vector<LaneSF> RoadSF::getLanesSf()
{
	std::vector<LaneSF> l = this->lanes.getLaneSF();
    return l;
}

std::vector<CenterLaneSF*> RoadSF::getCenterLaneSf ()
{
    return this->lanes.getCenterLanes();
}

std::vector<ObjectSF> RoadSF::getObjectsSf ()
{
    // CPLError(CE_Debug, CPLE_None, "RoadSF::getObjectsSf()");
    return this->objProc.getObjectsSF();
    
}

std::vector<SignalSF> RoadSF::getSignalSf ()
{
    // CPLError(CE_Debug, CPLE_None, "RoadSF::getSignalSf()");
    return sigProc.getSignalsSF();
}

road RoadSF::getRoad()
{
    // CPLError(CE_Debug, CPLE_None, "RoadSF::getRoad()");
    return this->od_road;
}
