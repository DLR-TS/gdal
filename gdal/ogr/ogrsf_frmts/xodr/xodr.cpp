/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   xodr.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 * 
 * Created on August 15, 2019, 2:29 PM
 */

#include "xodr.h"


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
 