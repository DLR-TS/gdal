/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Created:  February 2023
 * Modified: March 2023
 * Purpose:  Implementation of ogrxodrgeo.cpp
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center (DLR)		
 *           Gulsen Bardak, guelsen.bardak@dlr.de, German Aerospace Center (DLR)	
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
 
#include "ogr_xodr.h"
#include "geos/geom.h"
#include "geos/util.h"
//#include <OpenDrive/include/libopendrive.h>
//using namespace geos::geom;
//using namespace std;

//set Geometry Factory as global variable
//GeometryFactory *global_factory;
// TODO Create Geos/point for every single point inside geometries.  

//Coordinate Factory on different dimensions 

// Point Factory 
/*
unique_ptr<Point> PoiFactory(const CoordinateXY& coordinate){
    unique_ptr<Point> p = global_factory->createPoint(coordinate);
    return p;
    }

// LineString Factory 
unique_ptr<LineString> LSFactory(const CoordinateSequence& coordinates){
    unique_ptr<LineString> ls = global_factory->createLineString(coordinates);
    return ls;
    }
*/

class HeaderItems{
    public:
        int                 revMajor;
        int                 revMinor;
        std::string         name;
        double              version;
        long double         north;
        long double         south;
        long double         east;
        long double         west;      
        std::string         vendor;
    
    //private:
    //    void                getHeader();
    class GeoReference{
        std::string         proj;
        double              lat;
        double              lon;
        double              x0;
        double              y0;
        std::string         datum;

        int                 zone;
        std::string         ellps;

        //private:
        //void                getGeoReference();
        
    };    

};