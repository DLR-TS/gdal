/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of the calculator for OpenDRIVE plan view geometries.
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

#ifndef PLANVIEWCALCULATOR_H
#define PLANVIEWCALCULATOR_H

#include "PlanViewGeometry.h"
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

