/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of a wrapper to access OpenDRIVE OGC Simple Features.
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
    

#include "XODR.h"


/*
    C-Structor
*/
XODR::XODR(const char* pszFilename) 
{
    try
    {
		this->op = OpenDRIVE_(pszFilename, xml_schema::flags::keep_dom | xml_schema::flags::dont_validate);
    }
    catch(const std::exception& e)
    {
		std::cerr << e.what() << '\n';
    }
}

XODR::XODR()
{
}
/*
    D-Structor
*/
XODR::~XODR() {
  
}

/***
 * get minor revision number of opendrive file
 */
uint16_t XODR::getMinorRevision(){
    OpenDRIVE::header_type h = this->op->header();
    header::revMinor_optional& revMinor = h.revMinor();
    uint16_t rev    =   revMinor.get();
    return rev;
}
/***
 * get major revision number of opendrive file
 */
uint16_t XODR::getMajorRevision(){
    OpenDRIVE::header_type h = this->op->header();
    OpenDRIVE::header_type::revMinor_optional& revMajor = h.revMajor();
    uint16_t rev    =   revMajor.get();
    return rev;
}

std::string XODR::getSpatialReferenceSystem() {
    
	OpenDRIVE::header_type h = this->op->header();
    OpenDRIVE::header_type::geoReference_optional reference = h.geoReference();
	return reference.present() ? reference.get() : "";
}

/***
 * get all roads of opendrive file
**/
std::vector<RoadSF> XODR::getRoads()
{
    const OpenDRIVE::road_sequence& opRoads = op->road ();
    std::vector<RoadSF> sfroads;
    for(const OpenDRIVE::road_type r : opRoads)
    {
        RoadSF *rsf = new RoadSF(r);
        sfroads.push_back (*rsf);
    }
    return sfroads;
}

std::string XODR::getVendor() {
	OpenDRIVE::header_type h = this->op->header();
	OpenDRIVE::header_type::vendor_optional vendor = h.vendor();
	return vendor.present() ? vendor.get() : "";
}
