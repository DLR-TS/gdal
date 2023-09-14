#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

using namespace odr;
using namespace std;

OGRXODRLayerReferenceLine::OGRXODRLayerReferenceLine(
    RoadElements xodrRoadElements, std::string proj4Defn,
    bool dissolveTriangulatedSurface)
    : OGRXODRLayer(xodrRoadElements, proj4Defn,
                   dissolveTriangulatedSurface)
{
    this->featureDefn = new OGRFeatureDefn(FEATURE_CLASS_NAME.c_str());
    SetDescription(FEATURE_CLASS_NAME.c_str());
    featureDefn->Reference();
    featureDefn->GetGeomFieldDefn(0)->SetSpatialRef(&spatialRef);

    defineFeatureClass();
}

OGRFeature *OGRXODRLayerReferenceLine::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (roadIter != roadElements.roads.end())
    {
        feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

        const double eps = 0.9;
        odr::Road road = *roadIter;
        odr::RefLine refLine = road.ref_line;
        std::set<double> sVals =
            refLine.approximate_linear(eps, 0.0, road.length);

        OGRLineString lineString;
        for (const double &s : sVals)
        {
            odr::Vec3D refLineVertex = refLine.get_xyz(s);
            lineString.addPoint(refLineVertex[0], refLineVertex[1],
                                refLineVertex[2]);
        }
        OGRGeometry *geometry = lineString.MakeValid();

        feature->SetGeometry(geometry);
        feature->SetField("ID", road.id.c_str());
        feature->SetField("Length", road.length);
        feature->SetField("Junction", road.junction.c_str());
        feature->SetFID(nNextFID++);

        roadIter++;
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

void OGRXODRLayerReferenceLine::defineFeatureClass()
{
    featureDefn->SetGeomType(wkbLineString);

    OGRFieldDefn oFieldID("ID", OFTString);
    featureDefn->AddFieldDefn(&oFieldID);

    OGRFieldDefn oFieldLen("Length", OFTReal);
    featureDefn->AddFieldDefn(&oFieldLen);

    OGRFieldDefn oFieldJunction("Junction", OFTString);
    featureDefn->AddFieldDefn(&oFieldJunction);
}