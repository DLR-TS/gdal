/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of ReferenceLine layer.
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

OGRXODRLayerReferenceLine::OGRXODRLayerReferenceLine(
    RoadElements xodrRoadElements, std::string proj4Defn,
    bool dissolveTriangulatedSurface)
    : OGRXODRLayer(xodrRoadElements, proj4Defn, dissolveTriangulatedSurface)
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