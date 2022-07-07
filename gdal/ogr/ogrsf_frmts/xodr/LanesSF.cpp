/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of accumulated lane borders (center and regular lanes).
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
