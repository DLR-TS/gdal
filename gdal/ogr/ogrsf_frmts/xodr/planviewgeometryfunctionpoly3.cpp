/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "planviewgeometryfunction.h"

PlanViewGeometryFunctionPoly3::PlanViewGeometryFunctionPoly3(poly3 poly, double length):
    poly(CubicPolynomial(poly.a().get(),poly.b().get(),poly.c().get(),poly.d().get())),
        length(length)
{
    maxU = this->approximateU(this->length);
}

PlanViewGeometryFunctionPoly3::~PlanViewGeometryFunctionPoly3()
{

}

void PlanViewGeometryFunctionPoly3::calculateLocalCoordinate(Coordinate& p, double s) 
{
    double u,v;
    u = this->calculateU(s);
    v = this->calculateV(s);

    p.x = u;
    p.y = v;
}

void PlanViewGeometryFunctionPoly3::calculateLocalOffsetCoordinate(Coordinate& p, double s, double t) 
{

    double u = this->calculateU(s);
    double v = this->calculateV(s);

    double mTangent = this->getDerivedValue(u);
    double bTangent = v - mTangent * u;

    double tangentEndU = u + 1.0;
    double tangentEndV = mTangent * tangentEndU + bTangent;
    LineSegment tangent(u, v, tangentEndU, tangentEndV);
    tangent.pointAlongOffset(0.0, t, p);
}

double PlanViewGeometryFunctionPoly3::calculateU(double s) 
{
    double approxU = this->approximateU(s);
    return approxU;
}

double PlanViewGeometryFunctionPoly3::calculateV(double s) 
{
    double u = calculateU(s);
    double v;
    v = this->poly.value(u);
    return v;
}

double PlanViewGeometryFunctionPoly3::getDerivedValue(double s)
{
    CubicPolynomial derivative = this->poly.derivative();
    double u = derivative.value(s);
    return u;
}

/**
 * Performs a stepwise, numerical approximation of the curve length to determine the maximal possible u-coordinate
 * to not exceed the valid curve length during sampling.<p>
 * Author Ulf Noyer (ulf.noyer@dlr.de).
 *
 * @param poly The polynomial function to be used for approximation calculations.
 * @param requestedLength The polynomial's length for which to approximate the maximal valid u-coordinate.
 * @return The maximal u-coordinate which is valid for the polynomials requested length.
*/

double PlanViewGeometryFunctionPoly3::approximateU(double requestedLength)
{
    double u = 0.0;
    double v = this->poly.value(u);
    double approxCurveLength = 0.0;

    double stepSize = 0.01;
    double epsilon = 0.00001;

    while(fabs((requestedLength - approxCurveLength)) > epsilon)
    {
        const double newU = u + stepSize;
        const double newV = this->poly.value(newU);

        geos::geom::LineSegment stepSegment(u, v, newU, newV);
        const double stepLength = stepSegment.getLength();
        const double newLength = approxCurveLength + stepLength;

        if (newLength < requestedLength)
        {
            u = newU;
            v = newV;
            approxCurveLength = newLength;
        }
        else{
            stepSize = stepSize / 2;
        }
    }
    return u;
}
