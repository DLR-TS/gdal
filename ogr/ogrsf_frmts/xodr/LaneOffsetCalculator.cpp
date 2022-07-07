/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of the calculator for OpenDRIVE lane offsets.
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
