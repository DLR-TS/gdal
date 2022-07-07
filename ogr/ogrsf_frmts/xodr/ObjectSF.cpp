/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of Simple Features for OpenDRIVE objects.
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

#include "ObjectSF.h"

ObjectSF::ObjectSF(Point* point, object obj):
xodrObject(obj),
p(point)
{

}

ObjectSF::ObjectSF(const ObjectSF& orig):
xodrObject(orig.xodrObject),
p(orig.p)
{
}

ObjectSF::~ObjectSF()
{
}

const object &ObjectSF::getXodrObject() const {
    return xodrObject;
}

const Point *ObjectSF::getP() const {
    return p;
}
