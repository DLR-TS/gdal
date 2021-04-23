/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneSectionProcessor.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 26, 2019, 4:37 PM
 */

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

