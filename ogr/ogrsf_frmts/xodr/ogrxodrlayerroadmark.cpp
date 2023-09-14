#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

using namespace odr;
using namespace std;

OGRXODRLayerRoadMark::OGRXODRLayerRoadMark(VSILFILE *filePtr,
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

OGRFeature *OGRXODRLayerRoadMark::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (roadMarkIter != roadElements.roadMarks.end())
    {
        feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

        odr::RoadMark roadMark = *roadMarkIter;
        odr::Mesh3D roadMarkMesh = *roadMarkMeshIter;

        OGRTriangulatedSurface tin = triangulateSurface(roadMarkMesh);

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
        feature->SetField(featureDefn->GetFieldIndex("RoadID"),
                          roadMark.road_id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("LaneID"),
                          roadMark.lane_id);
        feature->SetField(featureDefn->GetFieldIndex("Type"),
                          roadMark.type.c_str());
        feature->SetFID(nNextFID++);

        roadMarkIter++;
        roadMarkMeshIter++;
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

void OGRXODRLayerRoadMark::defineFeatureClass()
{
    if (dissolveSurface)
    {
        featureDefn->SetGeomType(wkbPolygon);
    }
    else
    {
        featureDefn->SetGeomType(wkbTINZ);
    }

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    featureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
    featureDefn->AddFieldDefn(&oFieldLaneID);

    OGRFieldDefn oFieldType("Type", OFTString);
    featureDefn->AddFieldDefn(&oFieldType);
}