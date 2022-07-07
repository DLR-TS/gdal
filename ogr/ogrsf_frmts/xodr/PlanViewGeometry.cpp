/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE plan view geometries.
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

#include "PlanViewGeometry.h"
#include "geos/geom.h"

PlanViewGeometry::PlanViewGeometry(OpenDRIVE::road_type::planView_type::geometry_type xodrplanviewgeometry):
xodrPlanViewGeometry(xodrplanviewgeometry)
{
    if(xodrplanviewgeometry.line().present())
    {
        this->planViewGeometryFunction  =
            new PlanViewGeometryFunctionLine();
        
    }else if(xodrplanviewgeometry.arc().present())
    {
        this->planViewGeometryFunction  =
            new PlanViewGeometryFunctionArc(xodrplanviewgeometry.arc().get());   
        
    }else if(xodrplanviewgeometry.spiral().present())
    {
        this->planViewGeometryFunction =
                new PlanViewGeometryFunctionSpiral(xodrplanviewgeometry.spiral().get());
        
        
    }else if(xodrplanviewgeometry.paramPoly3().present())
    {
            this->planViewGeometryFunction  =
                new PlanViewGeometryFunctionParamPoly3(xodrplanviewgeometry.paramPoly3().get(),xodrplanviewgeometry.length().get());
            
    }else if(xodrplanviewgeometry.poly3().present())
    {
        this->planViewGeometryFunction  =
                new PlanViewGeometryFunctionPoly3(xodrplanviewgeometry.poly3().get(),xodrplanviewgeometry.length().get());
        
    }else
    {
        throw std::runtime_error("Unsupported Operations");
        
    }   
}
PlanViewGeometry::PlanViewGeometry(const PlanViewGeometry& other):
  xodrPlanViewGeometry (other.xodrPlanViewGeometry), planViewGeometryFunction (other.planViewGeometryFunction) 
{ 

}
PlanViewGeometry::~PlanViewGeometry() 
{
}

Coordinate PlanViewGeometry::getInertialPoint(double s)
{
    Coordinate p;
    this->planViewGeometryFunction->calculateLocalCoordinate (p,s);
    MatrixTransformations::initMatrix(this->m);
    MatrixTransformations::translate(this->m, xodrPlanViewGeometry.x().get(),xodrPlanViewGeometry.y().get());
    MatrixTransformations::rotate(this->m, xodrPlanViewGeometry.hdg().get());
    this->transformPoint (&p,this->m);
    return p;
}

void PlanViewGeometry::transformPoint(Coordinate* point,Matrix2D& m){
    Vector2D pt;
    pt.x = point->x;
    pt.y = point->y;
    pt.w = 1.0;
    MatrixTransformations::transform(pt, m);
    point->x = pt.x;
    point->y = pt.y;
}


Coordinate PlanViewGeometry::offsetPoint(double s, double t)
{
    Coordinate p(0.0,0.0);
    
    this->planViewGeometryFunction->calculateLocalOffsetCoordinate(p,s,t);
    
    MatrixTransformations::initMatrix(this->m);
    MatrixTransformations::translate(this->m, xodrPlanViewGeometry.x().get(),xodrPlanViewGeometry.y().get());
    MatrixTransformations::rotate(this->m, xodrPlanViewGeometry.hdg().get());
    
    this->transformPoint (&p,this->m);
    return p;
}


double PlanViewGeometry::getHdg() {
    return this->xodrPlanViewGeometry.hdg().get();
}

double PlanViewGeometry::getLength() {
    return this->xodrPlanViewGeometry.length().get();
}

double PlanViewGeometry::getX() {
    return this->xodrPlanViewGeometry.x().get();
}

double PlanViewGeometry::getY() {
    return this->xodrPlanViewGeometry.y().get();
}
