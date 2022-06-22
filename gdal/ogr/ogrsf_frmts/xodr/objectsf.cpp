//
// Created by opendrive on 05.09.19.
//

#include "objectsf.h"

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
