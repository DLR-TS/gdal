/******************************************************************************
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of RoadMark layer.
 * Author:   Michael Scholz, German Aerospace Center (DLR)
 *           Gülsen Bardak, German Aerospace Center (DLR)        
 *
 ******************************************************************************
 * Copyright 2024 German Aerospace Center (DLR), Institute of Transportation Systems
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_xodr.h"

OGRXODRLayerRoadMark::OGRXODRLayerRoadMark(const RoadElements& xodrRoadElements,
                                           std::string proj4Defn,
                                           bool dissolveTriangulatedSurface)
    : OGRXODRLayer(xodrRoadElements, proj4Defn, dissolveTriangulatedSurface)
{
    m_poFeatureDefn = std::make_unique<OGRFeatureDefn>(FEATURE_CLASS_NAME.c_str());
    SetDescription(FEATURE_CLASS_NAME.c_str());
    m_poFeatureDefn->Reference();
    m_poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(&m_poSRS);

    defineFeatureClass();
}

OGRFeature *OGRXODRLayerRoadMark::GetNextFeature()
{
    std::unique_ptr<OGRFeature> feature;

    if (m_roadMarkIter != m_roadElements.roadMarks.end())
    {
        feature = std::make_unique<OGRFeature>(m_poFeatureDefn.get());

        odr::RoadMark roadMark = *m_roadMarkIter;
        odr::Mesh3D roadMarkMesh = *m_roadMarkMeshIter;

   
        std::unique_ptr<OGRTriangulatedSurface> pTin = triangulateSurface(roadMarkMesh);
        OGRTriangulatedSurface tin = *pTin;
        if (m_bDissolveTIN)
        {
            OGRGeometry *dissolvedPolygon = tin.UnaryUnion();
            feature->SetGeometryDirectly(dissolvedPolygon);
        }
        else
        {
            //tin.MakeValid(); // TODO Works for TINs only with enabled SFCGAL support
            feature->SetGeometry(&tin);
        }
        feature->SetField(m_poFeatureDefn->GetFieldIndex("RoadID"),
                          roadMark.road_id.c_str());
        feature->SetField(m_poFeatureDefn->GetFieldIndex("LaneID"),
                          roadMark.lane_id);
        feature->SetField(m_poFeatureDefn->GetFieldIndex("Type"),
                          roadMark.type.c_str());
        feature->SetFID(m_nNextFID++);

        m_roadMarkIter++;
        m_roadMarkMeshIter++;
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
    if (m_bDissolveTIN)
    {
        OGRwkbGeometryType wkbPolygonWithZ = OGR_GT_SetZ(wkbPolygon);
        m_poFeatureDefn->SetGeomType(wkbPolygonWithZ);
    }
    else
    {
        m_poFeatureDefn->SetGeomType(wkbTINZ);
    }

    OGRFieldDefn oFieldRoadID("RoadID", OFTString);
    m_poFeatureDefn->AddFieldDefn(&oFieldRoadID);

    OGRFieldDefn oFieldLaneID("LaneID", OFTInteger);
    m_poFeatureDefn->AddFieldDefn(&oFieldLaneID);

    OGRFieldDefn oFieldType("Type", OFTString);
    m_poFeatureDefn->AddFieldDefn(&oFieldType);
}