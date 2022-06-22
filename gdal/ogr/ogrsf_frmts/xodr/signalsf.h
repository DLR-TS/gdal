//
// Created by opendrive on 05.09.19.
//

#ifndef GDAL_STANDALONE_CONVERT_SIGNALSF_H
#define GDAL_STANDALONE_CONVERT_SIGNALSF_H

#include "OpenDRIVE_1.4H.h"
#include "geos/geom.h"

using namespace geos::geom;


class SignalSF {
public:
    SignalSF(Point *point, signal signal);
    SignalSF(const SignalSF& orig);

    virtual ~SignalSF();

    const Point *getP() const;

    const signal &getXodrSignal() const;

private:
    Point* p;
    signal xodrSignal;
};


#endif //GDAL_STANDALONE_CONVERT_SIGNALSF_H
