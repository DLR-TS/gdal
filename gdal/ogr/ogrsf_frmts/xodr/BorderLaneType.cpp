/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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
