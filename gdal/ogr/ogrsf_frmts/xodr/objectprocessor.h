/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of the processor for OpenDRIVE objects.
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

#ifndef GDAL_STANDALONE_CONVERT_OBJECTPROCESSOR_H
#define GDAL_STANDALONE_CONVERT_OBJECTPROCESSOR_H

#include "objectsf.h"
#include "OpenDRIVE_1.4H.h"
#include "planviewcalculator.h"
#include "geos/geom.h"
#include <iostream>
#include "cpl_error.h"

class ObjectProcessor {

public:

    ObjectProcessor();
    ObjectProcessor(const OpenDRIVE::road_type::objects_type& objects,const PlanViewCalculator& pvc);
    ObjectProcessor(const ObjectProcessor& orig);
    virtual ~ObjectProcessor();
    std::vector<ObjectSF> getObjectsSF();

private:
    PlanViewCalculator pvc;
    OpenDRIVE::road_type::objects_type::object_sequence roadObjects;
    const GeometryFactory* gf = GeometryFactory::getDefaultInstance();

};


#endif //GDAL_STANDALONE_CONVERT_OBJECTPROCESSOR_H
