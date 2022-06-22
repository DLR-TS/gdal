/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   planviewgeometry.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 15, 2019, 3:55 PM
 */

#ifndef PLANVIEWGEOMETRY_H
#define PLANVIEWGEOMETRY_H



#include "planviewgeometryfunction.h"
#include "geos/geom.h"
#include "MatrixTransformations2D.h"
#include <iostream>
#include <math.h>

using namespace geos::geom;

class PlanViewGeometry {
public:
    PlanViewGeometry(OpenDRIVE::road_type::planView_type::geometry_type xodrplanviewgeometry);
    virtual ~PlanViewGeometry();
    PlanViewGeometry (const PlanViewGeometry& other);


    Coordinate getInertialPoint(double s);
    Coordinate offsetPoint(double s, double t);
    double getHdg();
    double getLength();
    double getX();
    double getY();
    
private:
    void transformPoint(Coordinate* geometry, Matrix2D& m);
    geometry xodrPlanViewGeometry;
    PlanViewGeometryFunction* planViewGeometryFunction;
    Matrix2D m;
};

#endif /* PLANVIEWGEOMETRY_H */

