//
// Created by opendrive on 07.10.19.
//

#include "signalprocessor.h"

SignalProcessor::SignalProcessor()
{}

SignalProcessor::SignalProcessor(const OpenDRIVE::road_type::signals_type& signals,const PlanViewCalculator& planViewCalculator):
pvc(planViewCalculator),
signalsType(signals.signal())
{}

SignalProcessor::SignalProcessor(const SignalProcessor& orig):
pvc(orig.pvc),
signalsType(orig.signalsType)
{}

SignalProcessor::~SignalProcessor() 
{}

std::vector<SignalSF> SignalProcessor::getSignalsSF()
{
    std::vector<SignalSF> signalVector;
    if(!this->signalsType.empty())
    {
        for(signal sig: this->signalsType)
        {
            double s = sig.s().get();
            double t = sig.t().get();
			if (s >= 0.0) {
				Coordinate signalPosition = this->pvc.offsetPoint(s, t);
				SignalSF signalSf(this->gf->createPoint(signalPosition), sig);
				signalVector.push_back(signalSf);
			}
        }
    }
    return signalVector;
}
