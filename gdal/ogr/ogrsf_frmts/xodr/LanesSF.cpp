/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LanesSF.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 23, 2019, 1:22 PM
 */

#include "LanesSF.h"


LanesSF::LanesSF (const PlanViewCalculator& planViewCalculator,OpenDRIVE::road_type::lanes_type *lanes, double roadLength):
road_lanes(*lanes),
pvc(PlanViewCalculator(planViewCalculator))
{
    this->loc = new LaneOffsetCalculator(road_lanes.laneOffset(), roadLength);
   for (uint16_t i = 0; i < road_lanes.laneSection().size (); i++)
    {
        double length = 0.0;
        if(i < road_lanes.laneSection ().size () - 1)
            length  = road_lanes.laneSection ().at (i + 1).s ().get () - road_lanes.laneSection().at(i).s().get();
        else
            length  = roadLength - road_lanes.laneSection().at(i).s().get();
        laneSections.push_back (LaneSectionSF(length,road_lanes.laneSection().at(i)));
        
    }
}

LanesSF::LanesSF(const LanesSF &orig):
    pvc(orig.pvc),
    laneSections(laneSections),
    loc(new LaneOffsetCalculator(*orig.loc))
{
    
}

LanesSF::~LanesSF () {
    delete(loc);
}

void LanesSF::createLanes()
{
    std::vector<LaneSF> tempLanes;
    for(LaneSectionSF lssf : laneSections)
    {
        tempLanes = lssf.createLanes (pvc, *loc);
        for (auto l : tempLanes)
        {
            lanesSF.push_back(l);
        }
    }
}

void LanesSF::createCenterLane()
{
    for (LaneSectionSF elem : laneSections) {
        this->centerlaneSF.push_back(elem.createCenterLanes(pvc, *loc));
    }
}

std::vector<LaneSF> LanesSF::getLaneSF()
{
    createLanes();
    return lanesSF;
}

std::vector<CenterLaneSF*> LanesSF::getCenterLanes()
{
    createCenterLane();
    return centerlaneSF;
}
