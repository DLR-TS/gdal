/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   xodr.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 15, 2019, 2:29 PM
 */

#ifndef XODR_H
#define XODR_H
#define USE_UNSTABLE_GEOS_CPP_API

#include "OpenDRIVE_1.4H.h"
//#include "cpl_error.h"
#include "roadsf.h"
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

