/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <map>

#include"LaneType.h"

WidthLaneType::WidthLaneType (OpenDRIVE::road_type::lanes_type::laneSection_type::left_type::lane_type lane, int orientationIdx):
laneID(lane.id ().get ()),
orientationIdx(orientationIdx)
{
  createLaneMap (&lane);
}

CubicPolynomial
WidthLaneType::getPolynomialFunctionForSOffset (double s) 
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
WidthLaneType::getValueForPolynomialFunctionForSOffset (double s) 
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

void
WidthLaneType::createLaneMap (lane* lane) 
{
    for(width width :lane->width ())
      {
        CubicPolynomial poly(width.a ().get (),width.b ().get (),width.c ().get (),width.d ().get ());
        this->roadlanes.insert (std::pair<double,CubicPolynomial>(width.sOffset ().get (),poly));
      }
}

double
WidthLaneType::getOffsetScopeBeginOfCurrentLaneTypeFunction (double s) {
  double begin = 0.0;
  for(auto const& lane : roadlanes)
    {
      if(s > lane.first)
        begin = lane.first;
    }
  return begin;
 }


WidthLaneType::~WidthLaneType () {
 }
