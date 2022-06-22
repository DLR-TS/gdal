/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include"planviewgeometryfunction.h"

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

