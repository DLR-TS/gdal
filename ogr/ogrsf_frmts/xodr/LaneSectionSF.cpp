/******************************************************************************
 * $Id$
 *
 * Project:  OpenGIS Simple Features for OpenDRIVE
 * Purpose:  Implementation of Simple Features for OpenDRIVE lane sections.
 * Author:   Michael Scholz, michael.scholz@dlr.de, German Aerospace Center (DLR)
 *           Oliver Böttcher, oliver.boettcher@dlr.de, German Aerospace Center (DLR)
 *
 ******************************************************************************
 * Copyright 2022 German Aerospace Center (DLR), Institute of Transportation Systems
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

#include "LaneSectionSF.h"


/**
 * 
 * Constructor
 * 
 * double length - 
 * laneSection section - 
 * 
 */ 
LaneSectionSF::LaneSectionSF (double length,
                              OpenDRIVE::road_type::lanes_type::laneSection_type section):
        section(section),
        length(length)
{
    
}

/**
 * Destructor
*/ 
LaneSectionSF::~LaneSectionSF ()
{

}

/*
    Sampling method for centerlane of road
*/
CenterLaneSF* LaneSectionSF::createCenterLanes (PlanViewCalculator& pvc, LaneOffsetCalculator& loc)
{
    
    double sectionEnd = section.s ().get () + length;
    double sampleRate = length / 1000;
    CoordinateArraySequence centerLineCoordinates;
       
    for (double s = section.s().get(); s < sectionEnd; s += sampleRate)
    {
        double x = s;
        double y = loc.getValueOfPolynomialFunctionForSOffset (s);
        centerLineCoordinates.add(Coordinate(pvc.offsetPoint (x,y)));
    }

    double y = loc.getValueOfPolynomialFunctionForSOffset (sectionEnd);
    centerLineCoordinates.add(Coordinate(pvc.offsetPoint (sectionEnd,y)));

    
    
    return new CenterLaneSF(centerLineCoordinates);
}

/*
   
*/
std::vector<LaneSF> LaneSectionSF::createLanes(PlanViewCalculator& pvc, LaneOffsetCalculator& loc)
{

    double sampleRate = length / 100;
    LaneSectionProcessor lsp(&section, loc, length, section.s ().get (), sampleRate);
    LineMap leftLaneLines;
    LineMap rightLaneLines;
    std::vector<LaneSF> laneSF;

    int orientationIdx = 1;
    leftLaneLines = createLineStringsForLanes (lsp, pvc, orientationIdx);
    orientationIdx = -1;
    rightLaneLines = createLineStringsForLanes (lsp, pvc, orientationIdx);
    if(leftLaneLines.size () > 0)
    {
        laneSection::left_type::lane_sequence lane_seq = section.left ().get ().lane ();
        for(auto lLane : leftLaneLines)
        {
            int id = lLane.first;
            for (auto sLane : lane_seq)
            {
				if (id == sLane.id().get()) {
					laneSF.push_back(LaneSF(*lLane.second, sLane));
				}
            }
			delete(lLane.second);
        }
    }
    if(rightLaneLines.size () > 0)
    {
        laneSection::right_type::lane_sequence lane_seq = section.right ().get ().lane ();
        for(auto& rLane : rightLaneLines)
        {
            int id = rLane.first;
            for (auto sLane : lane_seq)
            {
                if(id == sLane.id ().get ()){
                    laneSF.push_back(LaneSF(*rLane.second,sLane));
                }
            }
			delete(rLane.second);
        }
    }

    return laneSF;
}
/*
    creates linestrings from the coordinate arrays of the lanes
*/
LineMap LaneSectionSF::createLineStringsForLanes(LaneSectionProcessor& lsp, PlanViewCalculator& pvc, int orientation)
{
    LineMap offsetLanes;
    LaneCoordinateMap laneCoordinates = lsp.getCoordinatesOfTheLanes (orientation);

    if(laneCoordinates.size () > 0)
    {
        for(auto& laneCoords : laneCoordinates)
        {
            int id = laneCoords.first;
            CoordinateArraySequence offsetSeq;
            for(int i = 0; i < laneCoords.second.size(); ++i)
            {
                double x    = laneCoords.second.getAt(i).x;
                double y    = laneCoords.second.getAt(i).y * orientation;
                Coordinate c(pvc.offsetPoint(x,y));
                offsetSeq.add(c);
            }
            LineString* l = geometryFactory->createLineString(CoordinateArraySequence(offsetSeq));
            offsetLanes.insert(std::make_pair(id, l));
         
        }
    }
    return offsetLanes;
}
