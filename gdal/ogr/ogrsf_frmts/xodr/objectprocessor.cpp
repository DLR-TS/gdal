//
// Created by opendrive on 02.10.19.
//

#include "objectprocessor.h"

ObjectProcessor::ObjectProcessor()
{
}

ObjectProcessor::ObjectProcessor(const OpenDRIVE::road_type::objects_type& objects,const PlanViewCalculator& pvc):
pvc(pvc),
roadObjects(objects.object())
{

}

ObjectProcessor::ObjectProcessor(const ObjectProcessor& orig):
pvc(orig.pvc),
roadObjects(orig.roadObjects){}

ObjectProcessor::~ObjectProcessor() {}

std::vector<ObjectSF> ObjectProcessor::getObjectsSF()
{
    std::vector<ObjectSF> objects;
    if(!this->roadObjects.empty())
    {
        for(object obj: this->roadObjects)
        {
            double s = obj.s().get();
            double t = obj.t().get();
            if (s >= 0.0) {
                Coordinate objectPosition = this->pvc.offsetPoint(s, t);
                ObjectSF objs(this->gf->createPoint(objectPosition),obj);
                objects.push_back(objs);
            }
        }
    }
    return objects;
}
