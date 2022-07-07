/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of Simple Features for OpenDRIVE lane borders.
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

#ifndef LANESF_H
#define LANESF_H

#include "OpenDRIVE_1.4H.h"
#include <geos/geom.h>

using namespace geos::geom;

class LaneSF
{
public:

    LaneSF (LineString& laneLineString,lane& roadLane);
    LaneSF (const LaneSF& orig);
    virtual ~LaneSF ();
    lane GetRoadLane () const;
    LineString* getGeometry();

private:
    const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
    LineString* lineString;
    lane roadLane;

};

#endif /* LANESF_H */

