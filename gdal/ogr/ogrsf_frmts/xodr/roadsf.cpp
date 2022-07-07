/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of Simple Features for OpenDRIVE roads.
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center (DLR)
 *           Oliver Böttcher, oliver.boettcher@dlr.de, German Aerospace Center (DLR)
 *
 ******************************************************************************
 * Copyright 2022 German Aerospace Center (DLR), Institute of Transportation Systems
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

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
