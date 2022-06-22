//
// Created by opendrive on 05.09.19.
//

#ifndef GDAL_STANDALONE_CONVERT_OBJECTSF_H
#define GDAL_STANDALONE_CONVERT_OBJECTSF_H


#include <OpenDRIVE_1.4H.h>
#include "geos/geom.h"

using namespace geos::geom;

class ObjectSF {

public:
    ObjectSF(Point* point, object obj);
    ObjectSF(const ObjectSF& orig);
    virtual ~ObjectSF();

    const object &getXodrObject() const;
    const Point *getP() const;

private:
    Point* p;
    object xodrObject;

};


#endif //GDAL_STANDALONE_CONVERT_OBJECTSF_H
