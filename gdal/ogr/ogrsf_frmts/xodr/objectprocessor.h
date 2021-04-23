//
// Created by opendrive on 02.10.19.
//

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
