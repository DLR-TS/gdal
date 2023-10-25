/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of LaneBorder layer.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2023 German Aerospace Center (DLR), Institute of Transportation Systems
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

OGRXODRLayerLaneBorder::OGRXODRLayerLaneBorder(RoadElements xodrRoadElements,
                                               std::string proj4Defn)
    : OGRXODRLayer(xodrRoadElements, proj4Defn)
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
        for (auto vertexIter = laneOuter.begin();
             vertexIter != laneOuter.end(); ++vertexIter)
        {
            odr::Vec3D laneVertex = *vertexIter;
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