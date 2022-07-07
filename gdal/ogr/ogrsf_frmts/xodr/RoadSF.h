/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of Simple Features for OpenDRIVE roads.
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

#ifndef ROADSF_H
#define ROADSF_H

#include "OpenDRIVE_1.4H.h"
#include "PlanViewCalculator.h"
#include "ObjectSF.h"
#include "ObjectProcessor.h"
#include "SignalSF.h"
#include "SignalProcessor.h"
#include "LanesSF.h"
#include "LaneSF.h"
#include "geos/geom.h"
#include "geos_c.h"
#include "cpl_error.h"

using namespace xml_schema;
using namespace geos::geom;


class RoadSF {
public:
    RoadSF(const OpenDRIVE::road_type& r);
    RoadSF(const RoadSF& orig);
    virtual ~RoadSF();
    geos::geom::LineString* getPlanViewSF();
    OpenDRIVE::road_type getRoad();
    std::vector<LaneSF> getLanesSf();
    std::vector<CenterLaneSF*> getCenterLaneSf();
    std::vector<ObjectSF> getObjectsSf();
    std::vector<SignalSF> getSignalSf();
    
    
private:
    const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
    OpenDRIVE::road_type od_road;
    PlanViewCalculator pvc;
    ObjectProcessor objProc;
    SignalProcessor sigProc;
    LanesSF lanes;
};

#endif /* ROADSF_H */

