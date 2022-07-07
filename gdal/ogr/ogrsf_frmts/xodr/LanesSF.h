/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of accumulated lane borders (center and regular lanes).
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

