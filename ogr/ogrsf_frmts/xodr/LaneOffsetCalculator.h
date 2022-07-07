/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of the calculator for OpenDRIVE lane offsets.
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

#ifndef LANEOFFSETCALCULATOR_H
#define LANEOFFSETCALCULATOR_H

#include "OpenDRIVE_1.4H.h"
#include "CubicPolynomials.h"
#include <vector>
#include <iostream>
#include "cpl_error.h"

class LaneOffsetCalculator
{
public:
    LaneOffsetCalculator(const OpenDRIVE::road_type::lanes_type::laneOffset_sequence& offsets, double roadLength);
    LaneOffsetCalculator(const LaneOffsetCalculator& orig);
    virtual ~LaneOffsetCalculator ();
    double getValueOfPolynomialFunctionForSOffset(double s);
private:
    std::map<double,CubicPolynomial*> LaneOffsets;
    double roadLength;
  
    void createLaneOffsetsForRoad(const OpenDRIVE::road_type::lanes_type::laneOffset_sequence& offsets);

};

#endif /* LANEOFFSETCALCULATOR_H */

