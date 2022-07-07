/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE plan view spiral/clothoid geometry functions.
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

#include "planviewgeometryfunction.h"

PlanViewGeometryFunctionSpiral::PlanViewGeometryFunctionSpiral(spiral _spiral)
{
    
}

PlanViewGeometryFunctionSpiral::~PlanViewGeometryFunctionSpiral() {}

void PlanViewGeometryFunctionSpiral::calculateLocalCoordinate(Coordinate& p, double s) 
{

}

void PlanViewGeometryFunctionSpiral::calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) 
{

}

double PlanViewGeometryFunctionSpiral::calculateU(double s) 
{
	return 0.0;
}

double PlanViewGeometryFunctionSpiral::calculateV(double s) 
{
	return 0.0;
}
