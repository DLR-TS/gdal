/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Definition of OpenDRIVE plan view geometries.
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

#ifndef PLANVIEWGEOMETRY_H
#define PLANVIEWGEOMETRY_H



#include "PlanViewGeometryFunction.h"
#include "geos/geom.h"
#include "MatrixTransformations2D.h"
#include <iostream>
#include <math.h>

using namespace geos::geom;

class PlanViewGeometry {
public:
    PlanViewGeometry(OpenDRIVE::road_type::planView_type::geometry_type xodrplanviewgeometry);
    virtual ~PlanViewGeometry();
    PlanViewGeometry (const PlanViewGeometry& other);


    Coordinate getInertialPoint(double s);
    Coordinate offsetPoint(double s, double t);
    double getHdg();
    double getLength();
    double getX();
    double getY();
    
private:
    void transformPoint(Coordinate* geometry, Matrix2D& m);
    geometry xodrPlanViewGeometry;
    PlanViewGeometryFunction* planViewGeometryFunction;
    Matrix2D m;
};

#endif /* PLANVIEWGEOMETRY_H */

