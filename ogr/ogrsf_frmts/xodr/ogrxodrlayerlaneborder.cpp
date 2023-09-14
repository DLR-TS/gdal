#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

using namespace odr;
using namespace std;

OGRXODRLayerLaneBorder::OGRXODRLayerLaneBorder(VSILFILE *filePtr,
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

OGRFeature *OGRXODRLayerLaneBorder::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (laneIter != roadElements.lanes.end())
    {
        feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

        odr::Lane lane = *laneIter;
        odr::Line3D laneOuter = *laneLinesOuterIter;
        std::string laneRoadID = *laneRoadIDIter;

        OGRLineString lineString;
        for (auto laneOuterIter = laneOuter.begin();
             laneOuterIter != laneOuter.end(); ++laneOuterIter)
        {
            odr::Vec3D laneVertex = *laneOuterIter;
            lineString.addPoint(laneVertex[0], laneVertex[1], laneVertex[2]);
        }
        OGRGeometry *geometry = lineString.MakeValid();

        feature->SetGeometry(geometry);
        feature->SetField(featureDefn->GetFieldIndex("RoadID"),
                          laneRoadID.c_str());
        feature->SetField(featureDefn->GetFieldIndex("ID"), lane.id);
        feature->SetField(featureDefn->GetFieldIndex("Type"),
                          lane.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Predecessor"),
                          lane.predecessor);
        feature->SetField(featureDefn->GetFieldIndex("Successor"),
                          lane.successor);
        feature->SetFID(nNextFID++);

        laneIter++;
        laneLinesOuterIter++;
        laneLinesInnerIter++;  // For consistency, even though not used here
        laneRoadIDIter++;
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

void OGRXODRLayerLaneBorder::defineFeatureClass()
{
    featureDefn->SetGeomType(wkbLineString);

    OGRFieldDefn oFieldID("ID", OFTInteger);
    featureDefn->AddFieldDefn(&oFieldID);

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    featureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldType("Type", OFTString);
    featureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldPred("Predecessor", OFTInteger);
    featureDefn->AddFieldDefn(&oFieldPred);

    OGRFieldDefn oFieldSuc("Successor", OFTInteger);
    featureDefn->AddFieldDefn(&oFieldSuc);
}