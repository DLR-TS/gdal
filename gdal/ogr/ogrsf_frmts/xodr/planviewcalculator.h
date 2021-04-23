/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   planviewcalculator.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 15, 2019, 3:54 PM
 */

#ifndef PLANVIEWCALCULATOR_H
#define PLANVIEWCALCULATOR_H

#include "planviewgeometry.h"
#include <geos_c.h>
#include "OpenDRIVE_1.4H.h"
#include "cpl_error.h"
#include <iostream>
#include <map>
#include <utility>

using namespace geos::geom;

class PlanViewCalculator {
  
public:
    PlanViewCalculator();
    PlanViewCalculator(const OpenDRIVE::road_type::planView_type& planView);
    PlanViewCalculator(const PlanViewCalculator& orig);
    virtual ~PlanViewCalculator();
    Coordinate getPlanViewPoint(double s);
    Coordinate offsetPoint(double s, double t);

private:
     std::map<double,PlanViewGeometry> planViewGeometries;
     
     PlanViewGeometry getPlanViewGeometryForOffset(double* planviewOffset, double s);
};

#endif /* PLANVIEWCALCULATOR_H */

