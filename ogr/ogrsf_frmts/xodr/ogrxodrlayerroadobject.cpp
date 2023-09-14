#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

using namespace odr;
using namespace std;

OGRXODRLayerRoadObject::OGRXODRLayerRoadObject(VSILFILE *filePtr,
                                               RoadElements xodrRoadElements,
                                               std::string proj4Defn,
                                               bool dissolveTriangulatedSurface)
    : OGRXODRLayer(filePtr, xodrRoadElements, proj4Defn,
                   dissolveTriangulatedSurface)
{
    this->featureDefn = new OGRFeatureDefn(FEATURE_CLASS_NAME.c_str());
    SetDescription(FEATURE_CLASS_NAME.c_str());
    featureDefn->Reference();
    featureDefn->GetGeomFieldDefn(0)->SetSpatialRef(&spatialRef);
    
    defineFeatureClass();
}

OGRFeature *OGRXODRLayerRoadObject::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (roadObjectIter != roadElements.roadObjects.end())
    {
        feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

        odr::RoadObject roadObject = *roadObjectIter;
        odr::Mesh3D roadObjectMesh = *roadObjectMeshesIter;

        OGRTriangulatedSurface tin = triangulateSurface(roadObjectMesh);

        if (dissolveSurface)
        {
            OGRGeometry *dissolvedPolygon = tin.UnaryUnion();
            feature->SetGeometry(dissolvedPolygon);
        }
        else
        {
            //tin.MakeValid(); // TODO Works for TINs only with enabled SFCGAL support
            feature->SetGeometry(&tin);
        }

        feature->SetField(featureDefn->GetFieldIndex("ObjectID"),
                          roadObject.id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"),
                          roadObject.road_id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Type"),
                          roadObject.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Name"),
                          roadObject.name.c_str());
        feature->SetFID(nNextFID++);

        roadObjectIter++;
        roadObjectMeshesIter++;
    }

    if (feature)
    {
        return feature.release();
    }
    else
    {
        // End of features for the given layer reached.
        return nullptr;
    }
}

void OGRXODRLayerRoadObject::defineFeatureClass()
{
    if (dissolveSurface)
    {
        featureDefn->SetGeomType(wkbPolygon);
    }
    else
    {
        featureDefn->SetGeomType(wkbTINZ);
    }

    OGRFieldDefn oFieldObjectID("ObjectID", OFTString);
    featureDefn->AddFieldDefn(&oFieldObjectID);

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    featureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldType("Type", OFTString);
    featureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldObjectName("Name", OFTString);
    featureDefn->AddFieldDefn(&oFieldObjectName);
}