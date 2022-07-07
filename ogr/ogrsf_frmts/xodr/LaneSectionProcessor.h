/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of the processor for OpenDRIVE lane sections.
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

#ifndef LANESECTIONPROCESSOR_H
#define LANESECTIONPROCESSOR_H

#include "LaneOffsetCalculator.h"
#include "OpenDRIVE_1.4H.h"
#include "LaneType.h"
#include <memory.h>
#include <mutex>
#include "geos/geom.h"
#include "iostream"

using namespace geos::geom;

struct cmpByAbsIdValue {
  bool operator()(const int a, const int b) const {
    return abs (a) < abs(b);
  }
};

typedef std::map<int, CoordinateArraySequence, cmpByAbsIdValue> LaneCoordinateMap;
typedef std::map<int, std::unique_ptr<LaneType>, cmpByAbsIdValue> LaneMap;

class LaneSectionProcessor
{
public:
  LaneSectionProcessor (OpenDRIVE::road_type::lanes_type::laneSection_type* section,
                        LaneOffsetCalculator& loc, double length, double sectionOffset,
                        double sampleRate);

  virtual ~LaneSectionProcessor ();

    LaneCoordinateMap getCoordinatesOfTheLanes(int orientation);
  
private:
  
    LaneMap leftLanes;
    LaneMap rightLanes;
  
    double length;
    double sOffset;
    double sampleRate;
    LaneOffsetCalculator loc;
    
    void extractLanesForCorrectLaneSide(laneSection* section);
    LaneCoordinateMap discretizeLaneTypeToLineString(LaneMap* laneTypeLanes);
    LaneCoordinateMap accumulateWidthOfLanes(LaneCoordinateMap rawLaneCoordinates, int orientation);
    LaneCoordinateMap accumulateBorderOfLanes(LaneCoordinateMap rawLaneCoordinates, int orientation);
};

#endif /* LANESECTIONPROCESSOR_H */

