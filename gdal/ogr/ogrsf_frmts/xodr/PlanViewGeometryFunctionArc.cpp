/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE plan view arc geometry functions.
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

PlanViewGeometryFunctionArc::PlanViewGeometryFunctionArc(arc _arc):
curvature(_arc.curvature().get()),
    radius(1.0/std::abs(_arc.curvature().get()))
{
    
}
PlanViewGeometryFunctionArc::~PlanViewGeometryFunctionArc() {}

void PlanViewGeometryFunctionArc::calculateLocalCoordinate(Coordinate& p, double s) 
{
    double u = 0.0;
    double v = 0.0;
    double alpha = s / this->radius;

    u = this->radius * sin(alpha);
    if(this->curvature > 0)
        v = this->radius * -cos(alpha);
    else
        v = this->radius * cos(alpha);

    double yt = curvature > 0 ? this->radius : -this->radius;
    Matrix2D m;
    MatrixTransformations::initMatrix(m);
    MatrixTransformations::translate(m,0.0,yt);
    Vector2D vector2D;
    vector2D.x = u; vector2D.y = v; vector2D.w = 0.0;
    MatrixTransformations::transform(vector2D,m);
    p.x = vector2D.x;
    p.y = vector2D.y;
}

void PlanViewGeometryFunctionArc::calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) 
{
    double alpha = s / this->radius;
    double r = curvature > 0 ? radius - t  : radius + t ;
    double yt = curvature > 0 ? radius : -radius;
    double u = r * sin(alpha);
    double v;
    if(this->curvature > 0){
        v = r * -cos(alpha) + yt;
    }
    else{
        v = r *  cos(alpha) + yt;
    }
    p.x = u;
    p.y = v;
}

double PlanViewGeometryFunctionArc::calculateU(double s) 
{
    double alpha = s / this->radius;
    double u = this->radius * sin(alpha);
    return u;
}

double PlanViewGeometryFunctionArc::calculateV(double s) 
{
    double alpha = s / this->radius;
    double yt = curvature > 0 ? radius : -radius;
    double v;
    if(this->curvature > 0)
        v = this->radius * -cos(alpha) + yt;
    else
        v = this->radius *  cos(alpha) + yt;
    return v;
}

