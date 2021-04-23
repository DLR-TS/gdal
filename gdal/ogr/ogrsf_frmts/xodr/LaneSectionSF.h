/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LaneSectionSF.h
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 23, 2019, 1:40 PM
 */

#ifndef LANESECTIONSF_H
#define LANESECTIONSF_H

#include <vector>
#include "LaneSF.h"
#include "geos/geom.h"
#include "CenterLaneSF.h"
#include "OpenDRIVE_1.4H.h"
#include "planviewcalculator.h"
#include "LaneOffsetCalculator.h"
#include "LaneSectionProcessor.h"


using namespace geos::geom;
typedef std::map<int, LineString*, cmpByAbsIdValue> LineMap;


class LaneSectionSF
{
public:
    LaneSectionSF (double length, OpenDRIVE::road_type::lanes_type::laneSection_type section);
    virtual ~LaneSectionSF ();
    LineMap createLinesForLanes();
    std::vector<LaneSF> createLanes(PlanViewCalculator& pvc, LaneOffsetCalculator& loc);

    CenterLaneSF* createCenterLanes(PlanViewCalculator& pvc, LaneOffsetCalculator& loc);

private:
    double length;
    laneSection section;
    const GeometryFactory* geometryFactory = GeometryFactory::getDefaultInstance();
    LineMap createLineStringsForLanes(LaneSectionProcessor& lsp, PlanViewCalculator& pvc, int orientation);

};

#endif /* LANESECTIONSF_H */

