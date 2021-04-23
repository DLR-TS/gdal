/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   LaneSectionProcessor.cpp
 * Author: Oliver Boettcher(oliver.boettcher@dlr.de)
 *
 * Created on August 26, 2019, 4:37 PM
 */

#include "LaneSectionProcessor.h"

LaneSectionProcessor::LaneSectionProcessor (OpenDRIVE::road_type::lanes_type::laneSection_type* section,
                                            LaneOffsetCalculator& loc, double length, double sectionOffset,
                                            double sampleRate):
        loc(loc), sOffset(sectionOffset), length(length), sampleRate(sampleRate)
{

    extractLanesForCorrectLaneSide (section);

}

LaneSectionProcessor::~LaneSectionProcessor () { }

void LaneSectionProcessor::extractLanesForCorrectLaneSide(laneSection* section)
{
    if(section->left ().present ())
    {
        for(auto l : section->left ().get ().lane ()){
            if(l.border ().size () > 0)
                this->leftLanes.insert (
                        std::make_pair(l.id ().get (),std::make_unique<BorderLaneType>(l,1))
                );
            else if(l.width ().size () > 0)
                this->leftLanes.insert (
                        std::make_pair(l.id ().get (),std::make_unique<WidthLaneType>(l,1))
                );
        }
    }
    if(section->right ().present ())
    {
        for(auto const& l : section->right ().get ().lane ()){
            if(l.border ().size () > 0)
                this->rightLanes.insert (
                        std::make_pair(l.id ().get (),std::make_unique<BorderLaneType>(l,-1))
                );
            else if(l.width ().size () > 0)
                this->rightLanes.insert (
                        std::make_pair(l.id ().get (),std::make_unique<WidthLaneType>(l,-1))
                );
        }
    }
}


LaneCoordinateMap LaneSectionProcessor::getCoordinatesOfTheLanes(int orientation)
{
    LaneCoordinateMap coordinates;
    LaneMap* currentSide;
    bool width = false;
    currentSide = orientation == -1 ? &rightLanes : &leftLanes;

    LaneMap::iterator it = currentSide->begin ();
    for(;it != currentSide->end (); it++)
    {
        if(dynamic_cast<BorderLaneType *>(it->second.get()) != NULL)
        {
            width = false;
        }
        else if(dynamic_cast<WidthLaneType *>(it->second.get()) != NULL)
        {
            width = true;
        }
        break;
    }
    if(currentSide->size () > 0) {
        coordinates = discretizeLaneTypeToLineString(currentSide);

        if(width)
        {
            coordinates = accumulateWidthOfLanes (coordinates, orientation);
        }
        else{
//            coordinates = accumulateBorderOfLanes (coordinates, orientation);
        }
    }
    return coordinates;
}

LaneCoordinateMap LaneSectionProcessor::discretizeLaneTypeToLineString(LaneMap* laneTypeLanes)
{
    LaneCoordinateMap laneCoordinates;

    for(LaneMap::iterator it = laneTypeLanes->begin ();it != laneTypeLanes->end ();it++)
    {
        int laneID    = it->first;
        LaneType* lane = it->second.get ();
        double s = 0.0;
        double y = 0.0;
        double x = 0.0;
        CoordinateArraySequence seq;
        CubicPolynomial poly = lane->getPolynomialFunctionForSOffset (s);
        for(;s < this->length; s += this->sampleRate)
        {
            poly = lane->getPolynomialFunctionForSOffset (s);
            double innerS = s - lane->getOffsetScopeBeginOfCurrentLaneTypeFunction (s);
            x = s + this->sOffset;
            y = poly.value (innerS);
            Coordinate c(x, y);
            seq.add(c);
        }
        double lastPartialOffset = length - lane->getOffsetScopeBeginOfCurrentLaneTypeFunction (s);
        x = length + sOffset;
        y = poly.value (lastPartialOffset);
        Coordinate c(x, y);
        seq.add(c);

        laneCoordinates.insert (std::make_pair (laneID, seq));
    }
    return laneCoordinates;
}

LaneCoordinateMap
LaneSectionProcessor::accumulateBorderOfLanes (LaneCoordinateMap rawLaneCoordinates, int orientation) {
    LaneCoordinateMap accumulatedLaneCoordinates;
//  for(LineMap::iterator it = rawLaneLines.begin (); it != rawLaneLines.end (); it++)
//    {
//      int laneID    =   it->first;
//      OGRLineString laneLine = it->second;
//      OGRLineString accumulatedLine;
//      OGRPointIterator* lineit = laneLine.getPointIterator ();
//      OGRPoint p;
//        while(lineit->getNextPoint (&p))
//        {
//            double x = p.getX ();
//            double y = p.getY ();
//            double offset = loc.getValueOfPolynomialFunctionForSOffset (x);
//            if(offset != 0.0){
//                y = y + offset * orientation;
//            }
//            accumulatedLine.addPoint (x,y);
//        }
//
//      accumulatedLaneLines.insert (std::make_pair (laneID,accumulatedLine));
//    }
  return accumulatedLaneCoordinates;
}

LaneCoordinateMap
LaneSectionProcessor::accumulateWidthOfLanes (LaneCoordinateMap rawLaneCoordinates, int orientation) {
    LaneCoordinateMap accumulatedLaneCoordinates;

    for(LaneCoordinateMap::iterator it = rawLaneCoordinates.begin (); it != rawLaneCoordinates.end (); it++)
    {
        int laneID    =   it->first;

        CoordinateArraySequence seq;
        std::vector<Coordinate> laneCoordinates;
        it->second.toVector(laneCoordinates);

        if(it == rawLaneCoordinates.begin()) {
            for (std::vector<Coordinate>::iterator jt = laneCoordinates.begin();
                 jt != laneCoordinates.end();
                 jt++) {
                double offset = loc.getValueOfPolynomialFunctionForSOffset(jt->x);
                double y = jt->y + offset * orientation;
                seq.add(Coordinate(jt->x, y));
            }
            accumulatedLaneCoordinates.insert(std::make_pair(laneID, CoordinateArraySequence(seq)));
        }
        else
        {
            CoordinateArraySequence *prevLaneCoordinates;
            if(accumulatedLaneCoordinates.size () > 1)
                prevLaneCoordinates = new CoordinateArraySequence(accumulatedLaneCoordinates.rbegin()->second);
            else
                prevLaneCoordinates = new CoordinateArraySequence(accumulatedLaneCoordinates.begin ()->second);

            for (int i = 0; i < laneCoordinates.size(); i++) {
                double y = laneCoordinates.at(i).y + prevLaneCoordinates->getAt(i).y;
                seq.add(Coordinate(laneCoordinates.at(i).x, y));
            }
            accumulatedLaneCoordinates.insert(std::make_pair(laneID, CoordinateArraySequence(seq)));
            delete(prevLaneCoordinates);
        }
    }
    return accumulatedLaneCoordinates;
}

