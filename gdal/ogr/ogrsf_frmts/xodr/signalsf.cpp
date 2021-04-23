//
// Created by opendrive on 05.09.19.
//

#include "signalsf.h"

SignalSF::SignalSF(Point* point,signal signal):
        xodrSignal(signal),
        p(point)
{

}

SignalSF::SignalSF(const SignalSF& orig):
        xodrSignal(orig.xodrSignal),
        p(orig.p)
{
}

SignalSF::~SignalSF()
{
}

const Point *SignalSF::getP() const {
    return p;
}

const signal &SignalSF::getXodrSignal() const {
    return xodrSignal;
}
