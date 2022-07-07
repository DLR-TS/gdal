/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of OpenDRIVE plan view parametric cubic polynomial geometry functions.
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

#include <geos/geom/LineSegment.h>

#include "PlanViewGeometryFunction.h"

PlanViewGeometryFunctionParamPoly3::PlanViewGeometryFunctionParamPoly3(paramPoly3 poly, double length):
    length(length),
        polyU(CubicPolynomial(poly.aU().get(),poly.bU().get(),poly.cU().get(),poly.dU().get())),
            polyV(CubicPolynomial(poly.aV().get(),poly.bV().get(),poly.cV().get(),poly.dV().get()))
{
    isNormalized    =   true;
    if(poly.pRange().present())
    {
        pRange p = poly.pRange().get();
        isNormalized = p.empty() || p == pRange::normalized;
    }
}

PlanViewGeometryFunctionParamPoly3::~PlanViewGeometryFunctionParamPoly3() {}

void PlanViewGeometryFunctionParamPoly3::calculateLocalCoordinate(Coordinate& p, double s)
{
    double u,v;
    
    u = this->calculateU(s);
    v = this->calculateV(s);
    p.x = u;
    p.y = v;

}

void PlanViewGeometryFunctionParamPoly3::calculateLocalOffsetCoordinate(Coordinate& p, double s, double t)
{
    double u    =   calculateU(s);
    double v    =   calculateV(s);
    double dV   =   this->getDerivedValueV(s);
    double dU   =   this->getDerivedValueU(s);
    
    double tangentEndU;
    double tangentEndV;
    
    if(dU != 0.0)
    {
        double mTangent = dV / dU;
        double bTangent = v - mTangent * u;
        
        tangentEndU     =   dU > 0 ? u + 0.01 : u - 0.01;
        tangentEndV     =   mTangent *  tangentEndU + bTangent;
    } else {
        tangentEndU = u;
        if( dV > 0){
            tangentEndV =   v + 0.01;
        }else if(dV < 0){
            tangentEndV =   v - 0.01;
        }else{
            tangentEndV =   v;
        }
    }
    LineSegment* tangent = new LineSegment(u,v,tangentEndU,tangentEndV);
    
    Coordinate c;
    tangent->pointAlongOffset (0.0,t,c);
    p.x = c.x;
    p.y = c.y;
    delete(tangent);
}



double PlanViewGeometryFunctionParamPoly3::calculateU(double s)
{
    double validS   =   this->isNormalized ? s / this->length : s;
    double u        =   this->polyU.value(validS);
    return u;
    
}

double PlanViewGeometryFunctionParamPoly3::calculateV(double s)
{
    double validS   =   this->isNormalized ? s / this->length : s;
    double v        =   this->polyV.value(validS);
    return v;
}

double PlanViewGeometryFunctionParamPoly3::getDerivedValueU(double s) 
{   
    double validS   = this->isNormalized ? s / this->length : s;
    CubicPolynomial derivative  = this->polyU.derivative();
    double u    =   derivative.value(validS);
    return u;
    
}

double PlanViewGeometryFunctionParamPoly3::getDerivedValueV(double s)
{
    double validS   = this->isNormalized ? s / this->length : s;
    CubicPolynomial derivative  = this->polyV.derivative();
    double v    =   derivative.value(validS);
    return v;
    
}

void PlanViewGeometryFunctionParamPoly3::getFunctions(){
  std::cout<< "polyU: " << this->polyU.getA () << " + " <<this->polyU.getB () << "x + " << this->polyU.getC () << "x2 + " << this->polyU.getD () << "x3\n" ;
  std::cout<< "polyV: " << this->polyV.getA () << " + " <<this->polyV.getB () << "x + " << this->polyV.getC () << "x2 + " << this->polyV.getD () << "x3\n";
}
