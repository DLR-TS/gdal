/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of the processor for OpenDRIVE objects.
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

#include "objectprocessor.h"

ObjectProcessor::ObjectProcessor()
{
}

ObjectProcessor::ObjectProcessor(const OpenDRIVE::road_type::objects_type& objects,const PlanViewCalculator& pvc):
pvc(pvc),
roadObjects(objects.object())
{

}

ObjectProcessor::ObjectProcessor(const ObjectProcessor& orig):
pvc(orig.pvc),
roadObjects(orig.roadObjects){}

ObjectProcessor::~ObjectProcessor() {}

std::vector<ObjectSF> ObjectProcessor::getObjectsSF()
{
    std::vector<ObjectSF> objects;
    if(!this->roadObjects.empty())
    {
        for(object obj: this->roadObjects)
        {
            double s = obj.s().get();
            double t = obj.t().get();
            if (s >= 0.0) {
                Coordinate objectPosition = this->pvc.offsetPoint(s, t);
                ObjectSF objs(this->gf->createPoint(objectPosition),obj);
                objects.push_back(objs);
            }
        }
    }
    return objects;
}
