/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneOffsetCalculator.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 23, 2019, 1:24 PM
 */

#include "LaneOffsetCalculator.h"

LaneOffsetCalculator::LaneOffsetCalculator (const OpenDRIVE::road_type::lanes_type::laneOffset_sequence& offsets, double roadLength)
{
    this->roadLength = roadLength;

    this->createLaneOffsetsForRoad (offsets);
}

void LaneOffsetCalculator::createLaneOffsetsForRoad(const OpenDRIVE::road_type::lanes_type::laneOffset_sequence& offsets){
    
    for (int i = 0; i < offsets.size (); i++)
    {
      laneOffset offset = offsets.at (i);
      double a = offset.a ().get ();
      double b = offset.b ().get ();
      double c = offset.c ().get ();
      double d = offset.d ().get ();
      this->LaneOffsets.insert (
              std::pair<double,CubicPolynomial*>(offset.s ().get (),
                                                new CubicPolynomial(a,b,c,d)));
    }
    
}

LaneOffsetCalculator::LaneOffsetCalculator (const LaneOffsetCalculator& orig):
LaneOffsets(orig.LaneOffsets), roadLength(orig.roadLength)
{
    // CPLError(CE_Debug, CPLE_None, "LaneOffsetCalculator::createLaneOffsetsForRoad(const LaneOffsetCalculator& orig)");
}

LaneOffsetCalculator::~LaneOffsetCalculator ()
{
  
}

double LaneOffsetCalculator::getValueOfPolynomialFunctionForSOffset(double s)
{
	double value, innerS;
	CubicPolynomial* ptr = NULL;
	for (auto const& it : this->LaneOffsets)
	{
		if (s >= it.first) {
			ptr = it.second;
			innerS = it.first;

		}
	}
	if (ptr != NULL)
	{
		value = ptr->value(s - innerS);
		ptr = NULL;
	}
	delete ptr;
    return value;
}
