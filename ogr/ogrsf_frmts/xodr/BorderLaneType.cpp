/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE border lane type.
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

#include "LaneType.h"

BorderLaneType::BorderLaneType (OpenDRIVE::road_type::lanes_type::laneSection_type::left_type::lane_type lane, int orientationIdx):
laneID(lane.id ().get ()),
    orientationIdx(orientationIdx)
{
    createLaneMap (&lane);
}

void
BorderLaneType::createLaneMap (lane* lane) 
{
for(auto const& border:lane->border ())
      {
        CubicPolynomial poly(border.a ().get (),border.b ().get (),border.c ().get (),border.d ().get ());
        this->roadlanes.insert (std::pair<double,CubicPolynomial>(border.sOffset ().get (),poly));
      }
}


CubicPolynomial
BorderLaneType::getPolynomialFunctionForSOffset (double s) 
{
  std::auto_ptr<CubicPolynomial> ptr;
    for(auto const& x : this->roadlanes)
    {
        if(s >= x.first){
            ptr.reset (new CubicPolynomial(x.second));
        }
    }
  return *ptr.get ();
}

double
BorderLaneType::getValueForPolynomialFunctionForSOffset (double s) 
{
    double value;
    std::auto_ptr<CubicPolynomial> ptr;
    for(auto const& x : this->roadlanes)
    {
        if(s >= x.first){
            ptr.reset (new CubicPolynomial(x.second));
        }
    }
    value = ptr.get ()->value (s);
    return value;
}

double
BorderLaneType::getOffsetScopeBeginOfCurrentLaneTypeFunction (double s) {
  double begin = 0.0;
    for(auto const& lane : roadlanes)
    {
      if(s > lane.first)
        begin = lane.first;
    }
    return begin;
 }


BorderLaneType::~BorderLaneType () 
{

}
