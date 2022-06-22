/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/* 
 * File:   LanesSF.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 23, 2019, 1:22 PM
 */

#ifndef LANESSF_H
#define LANESSF_H

#include "planviewcalculator.h"
#include "LaneOffsetCalculator.h"
#include "OpenDRIVE_1.4H.h"
#include "cpl_error.h"
#include "LaneSectionSF.h"

class LanesSF
{
public:
    // LanesSF();
    LanesSF (const PlanViewCalculator& planViewCalculator,OpenDRIVE::road_type::lanes_type *lanes, double roadLength);
    LanesSF (const LanesSF& orig);
    virtual ~LanesSF ();


    std::vector<CenterLaneSF*> getCenterLanes();
    std::vector<LaneSF> getLaneSF();


private:
    OpenDRIVE::road_type::lanes_type road_lanes;
    PlanViewCalculator pvc;
    LaneOffsetCalculator* loc;
    std::vector<LaneSectionSF> laneSections;
    std::vector<LaneSF> lanesSF;
    std::vector<CenterLaneSF*> centerlaneSF;
    void createLanes();
    void createCenterLane();
};

#endif /* LANESSF_H */

