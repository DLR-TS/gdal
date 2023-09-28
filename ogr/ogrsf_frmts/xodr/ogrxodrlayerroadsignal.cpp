/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of RoadObject layer.
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


OGRXODRLayerRoadSignal::OGRXODRLayerRoadSignal(RoadElements xodrRoadElements,
                                               std::string proj4Defn,
                                               bool dissolveTriangulatedSurface)
    : OGRXODRLayer(xodrRoadElements, proj4Defn, dissolveTriangulatedSurface)
{
    this->featureDefn = new OGRFeatureDefn(FEATURE_CLASS_NAME.c_str());
    SetDescription(FEATURE_CLASS_NAME.c_str());
    featureDefn->Reference();
    featureDefn->GetGeomFieldDefn(0)->SetSpatialRef(&spatialRef);

    defineFeatureClass();
}


OGRFeature *OGRXODRLayerRoadSignal::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (roadSignalIter != roadElements.roadSignals.end())
    {
        feature = std::unique_ptr<OGRFeature>(new OGRFeature(featureDefn));

        odr::RoadSignal roadSignal = *roadSignalIter;
        odr::Mesh3D roadSignalMesh = *roadSignalMeshesIter;

        OGRTriangulatedSurface tin = triangulateSurface(roadSignalMesh);

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

        feature->SetField(featureDefn->GetFieldIndex("SignalID"),
                          roadSignal.id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("RoadID"),
                          roadSignal.road_id.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Type"),
                          roadSignal.type.c_str());
        feature->SetField(featureDefn->GetFieldIndex("Name"),
                          roadSignal.name.c_str());
        feature->SetFID(nNextFID++);

        roadSignalIter++;
        roadSignalMeshesIter++;
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

void OGRXODRLayerRoadSignal::defineFeatureClass()
{
    if (dissolveSurface)
    {
        featureDefn->SetGeomType(wkbPolygon);
    }
    else
    {
        featureDefn->SetGeomType(wkbTINZ);
    }

    OGRFieldDefn oFieldObjectID("SignalID", OFTString);
    featureDefn->AddFieldDefn(&oFieldObjectID);

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    featureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldType("Type", OFTString);
    featureDefn->AddFieldDefn(&oFieldType);

    OGRFieldDefn oFieldObjectName("Name", OFTString);
    featureDefn->AddFieldDefn(&oFieldObjectName);
}