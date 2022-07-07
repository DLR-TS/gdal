/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE plan view line geometry functions.
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

#include "PlanViewGeometryFunction.h"


PlanViewGeometryFunctionLine::PlanViewGeometryFunctionLine()
{
  
}

PlanViewGeometryFunctionLine::~PlanViewGeometryFunctionLine() {
}

void PlanViewGeometryFunctionLine::calculateLocalCoordinate(geos::geom::Coordinate& p, double s)
{
	p.x = s;
    p.y = 0;
}
void PlanViewGeometryFunctionLine::calculateLocalOffsetCoordinate(geos::geom::Coordinate& p, double s,double t)
{
  p.x = s;
  p.y = t;
}

double PlanViewGeometryFunctionLine::calculateV(double s)
{
    return 0.0;
}

double PlanViewGeometryFunctionLine::calculateU(double s)
{
    return s;
}

