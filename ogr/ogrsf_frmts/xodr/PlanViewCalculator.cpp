/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of the calculator for OpenDRIVE plan view geometries.
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

#include "PlanViewCalculator.h"

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
