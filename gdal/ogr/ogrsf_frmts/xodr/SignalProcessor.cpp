/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of the processor for OpenDRIVE signals.
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

#include "SignalProcessor.h"

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
