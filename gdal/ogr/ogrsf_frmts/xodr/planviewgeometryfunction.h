/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   planviewgeometryfunction.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 15, 2019, 3:55 PM
 */

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

