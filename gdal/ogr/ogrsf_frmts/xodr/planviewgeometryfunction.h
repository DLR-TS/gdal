/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of OpenDRIVE plan view geometry functions.
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


#ifndef PLANVIEWGEOMETRYFUNCTION_H
#define PLANVIEWGEOMETRYFUNCTION_H

#include "geos/geom.h"
#include "OpenDRIVE_1.4H.h"
#include "CubicPolynomials.h"
#include "MatrixTransformations2D.h"
#include <math.h>
#include <iostream>

using namespace geos::geom;

class PlanViewGeometryFunction 
{
public:
    virtual ~PlanViewGeometryFunction(){};
    virtual void calculateLocalCoordinate(Coordinate& p, double s) = 0;
    virtual void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) = 0;
    virtual double calculateU(double s) = 0;
    virtual double calculateV(double s) = 0;
};


class PlanViewGeometryFunctionLine : public PlanViewGeometryFunction
{
public:
    PlanViewGeometryFunctionLine();
    ~PlanViewGeometryFunctionLine();

    void calculateLocalCoordinate(Coordinate& p, double s) override;
    void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) override;
    double calculateV(double s) override;
    double calculateU(double s) override;


private:
};

class PlanViewGeometryFunctionPoly3 : public PlanViewGeometryFunction
{
public:
    PlanViewGeometryFunctionPoly3(poly3 poly, double length);
    ~PlanViewGeometryFunctionPoly3();
    void calculateLocalCoordinate(Coordinate& p, double s) override;
    void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) override;
    double calculateU(double s) override;
    double calculateV(double s) override;
    
private:
    double length;
    CubicPolynomial poly;
    double maxU;

    double approximateU(double requestedLength);
    double getDerivedValue(double s);
    
};

class PlanViewGeometryFunctionParamPoly3 : public PlanViewGeometryFunction
{
public:
    PlanViewGeometryFunctionParamPoly3(paramPoly3 poly, double length);
    ~PlanViewGeometryFunctionParamPoly3();
    void calculateLocalCoordinate(Coordinate& p, double s) override;
    void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) override;
    double calculateU(double s) override;
    double calculateV(double s) override;
    
private:
    CubicPolynomial polyU;
    CubicPolynomial polyV;
    double length;
    bool isNormalized;
    
    double getDerivedValueU(double s);
    double getDerivedValueV(double s);
    void getFunctions();
};

class PlanViewGeometryFunctionArc : public PlanViewGeometryFunction
{
public:
    PlanViewGeometryFunctionArc(arc _arc);
    ~PlanViewGeometryFunctionArc();
    void calculateLocalCoordinate(Coordinate& p, double s) override;
    void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) override;
    double calculateU(double s) override;
    double calculateV(double s) override;
private:
    const double curvature;
    const double radius;
    
};

class PlanViewGeometryFunctionSpiral : public PlanViewGeometryFunction
{
public:
    PlanViewGeometryFunctionSpiral(spiral _spiral);

    ~PlanViewGeometryFunctionSpiral();
    void calculateLocalCoordinate(Coordinate& p, double s) override;
    void calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) override;
    double calculateU(double s) override;
    double calculateV(double s) override;
    
};

#endif /* PLANVIEWGEOMETRYFUNCTION_H */

