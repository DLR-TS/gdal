/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of a wrapper to access OpenDRIVE OGC Simple Features.
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

#ifndef XODR_H
#define XODR_H
#define USE_UNSTABLE_GEOS_CPP_API

#include "OpenDRIVE_1.4H.h"
//#include "cpl_error.h"
#include "RoadSF.h"
#include <iostream>
#include <vector>



class XODR {
    public:
        XODR(const char * pszFilename);
		XODR();
        virtual ~XODR();
        uint16_t getMinorRevision();
        uint16_t getMajorRevision();
		std::string getSpatialReferenceSystem();
		std::string getVendor();
        std::vector<RoadSF> getRoads();
//        std::vector<lanesf> getLanes();
    private:
        std::auto_ptr<OpenDRIVE> op;


};

#endif /* XODR_H */

