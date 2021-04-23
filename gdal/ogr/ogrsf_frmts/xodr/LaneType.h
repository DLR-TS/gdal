/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneType.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 26, 2019, 4:46 PM
 */

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

