//
// Created by opendrive on 07.10.19.
//

#ifndef GDAL_STANDALONE_CONVERT_SIGNALPROCESSOR_H
#define GDAL_STANDALONE_CONVERT_SIGNALPROCESSOR_H

#include "geos/geom.h"
#include "OpenDRIVE_1.4H.h"
#include "planviewcalculator.h"
#include "signalsf.h"


class SignalProcessor {
public:
    SignalProcessor();
    SignalProcessor(const OpenDRIVE::road_type::signals_type &signals,const PlanViewCalculator &planViewCalculator);
    SignalProcessor(const SignalProcessor& orig);
    virtual ~SignalProcessor();

    std::vector<SignalSF> getSignalsSF();
private:
    PlanViewCalculator pvc;
    OpenDRIVE::road_type::signals_type::signal_sequence signalsType;
    const  GeometryFactory* gf = GeometryFactory::getDefaultInstance();

};


#endif //GDAL_STANDALONE_CONVERT_SIGNALPROCESSOR_H
