/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   planviewcalculator.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 15, 2019, 3:54 PM
 */

#include "planviewcalculator.h"

PlanViewCalculator::PlanViewCalculator(const OpenDRIVE::road_type::planView_type& planView):
planViewGeometries(std::map<double, PlanViewGeometry>())
{
    OpenDRIVE::road_type::planView_type::geometry_sequence xodrPlanViewGeometries = planView.geometry();
    for (OpenDRIVE::road_type::planView_type::geometry_type geometry 
            : xodrPlanViewGeometries) {
        PlanViewGeometry geom(geometry);
        double s = geometry.s ().get ();
        planViewGeometries.insert (std::pair<double,PlanViewGeometry>(s,geom));
    }
}
PlanViewCalculator::PlanViewCalculator()
{
}

PlanViewCalculator::PlanViewCalculator(const PlanViewCalculator& orig):
planViewGeometries(orig.planViewGeometries)
{
    
}

PlanViewCalculator::~PlanViewCalculator()
{
    
}

Coordinate PlanViewCalculator::getPlanViewPoint(double s)
{
    Coordinate p(0.0,0.0);
    double sOffset = 0.0;
    PlanViewGeometry pvg = this->getPlanViewGeometryForOffset(&sOffset, s);
    p   =   pvg.getInertialPoint (s - sOffset);
    return p;
  
}

Coordinate PlanViewCalculator::offsetPoint(double s, double t)
{
    double sOffset = 0.0;
    PlanViewGeometry pvg = this->getPlanViewGeometryForOffset(&sOffset, s);
    Coordinate p  =   pvg.offsetPoint (s - sOffset, t);
    return p;
}

PlanViewGeometry PlanViewCalculator::getPlanViewGeometryForOffset(double* planviewOffset, double s)
{
    std::auto_ptr<PlanViewGeometry> ptr;
    for(auto const& x : this->planViewGeometries)
    {
        if(s >= x.first){
            ptr.reset (new PlanViewGeometry(x.second));
            *planviewOffset = x.first;
        }
    }
    return *ptr.release ();
}
