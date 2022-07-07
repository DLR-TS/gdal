/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of the processor for OpenDRIVE signals.
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

#ifndef GDAL_STANDALONE_CONVERT_SIGNALPROCESSOR_H
#define GDAL_STANDALONE_CONVERT_SIGNALPROCESSOR_H

#include "geos/geom.h"
#include "OpenDRIVE_1.4H.h"
#include "PlanViewCalculator.h"
#include "SignalSF.h"


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
