/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of OpenDRIVE lane types.
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

#ifndef LANETYPE_H
#define LANETYPE_H

#include "CubicPolynomials.h"
#include "OpenDRIVE_1.4H.h"



class LaneType
{
public:
    virtual CubicPolynomial getPolynomialFunctionForSOffset(double s) = 0;
    virtual ~LaneType() {};
    virtual double getValueForPolynomialFunctionForSOffset(double s) = 0;
    virtual double getOffsetScopeBeginOfCurrentLaneTypeFunction(double s) = 0;
private:    
    virtual void createLaneMap(lane* lane) = 0;
  
};

class WidthLaneType : public LaneType
{
public:
    WidthLaneType (OpenDRIVE::road_type::lanes_type::laneSection_type::left_type::lane_type lane,
    int orientationIdx);
    
    virtual
    ~WidthLaneType ();

    CubicPolynomial getPolynomialFunctionForSOffset (double s) override;
    double getValueForPolynomialFunctionForSOffset (double s) override;
    double getOffsetScopeBeginOfCurrentLaneTypeFunction (double s) override;

private:
    std::map<double,CubicPolynomial> roadlanes;
    int laneID;
    int orientationIdx;

    void createLaneMap(lane* lane) override;
    
};

class BorderLaneType : public LaneType
{
public:
    BorderLaneType (OpenDRIVE::road_type::lanes_type::laneSection_type::left_type::lane_type lane,
        int orientationIdx);
    virtual
    ~BorderLaneType ();

    
    CubicPolynomial getPolynomialFunctionForSOffset (double s) override;
    double getValueForPolynomialFunctionForSOffset (double s) override;
    double getOffsetScopeBeginOfCurrentLaneTypeFunction (double s) override;

private:
    std::map<double,CubicPolynomial> roadlanes;
    int laneID;
    int orientationIdx;

    void createLaneMap(lane* lane) override;
};

#endif /* LANETYPE_H */

