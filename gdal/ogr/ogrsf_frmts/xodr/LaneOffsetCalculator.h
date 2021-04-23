/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneOffsetCalculator.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 23, 2019, 1:24 PM
 */

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

