/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "planviewgeometryfunction.h"


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

