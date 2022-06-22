#include "ogr_xodr.h"
#include "ogr_api.h"
#include "ogr_geometry.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include "cpl_error.h"


OGRXODRLayer::OGRXODRLayer(XODR *xodr, const std::vector<RoadSF> &roads, const char* pszName, uint8_t lId) :
	xodr(xodr),
	simpleRoads(roads),
	roadIter(simpleRoads.begin()),
	poFeatureDefn(new OGRFeatureDefn(CPLGetBasename(pszName)))
{
	layerId = lId;
	if(layerId == refLayer || layerId == laneLayer)
		poFeatureDefn->SetGeomType(OGRwkbGeometryType::wkbMultiLineString);
	else if(layerId == objectLayer || layerId == signalLayer)
		poFeatureDefn->SetGeomType(OGRwkbGeometryType::wkbPoint);
	else
		poFeatureDefn->SetGeomType(OGRwkbGeometryType::wkbMultiLineString);
	poFeatureDefn->Reference();

	ResetReading();
	poSRS = NULL;
	if (xodr->getMinorRevision() > 3) {
		std::string geoReference = xodr->getSpatialReferenceSystem();
        if (!geoReference.empty())
		{
			poSRS = new OGRSpatialReference();
			poSRS->importFromProj4(geoReference.c_str());

			poFeatureDefn->GetGeomFieldDefn(0)->SetSpatialRef(poSRS);

            //CPLError(CE_Warning, CPLE_AppDefined, geoReference.c_str());
		}
	}
	createRequestedLayer();

}

OGRXODRLayer::~OGRXODRLayer()
{
	if (poFeatureDefn != NULL)
	{
		poFeatureDefn->Release();
	}
	if (poSRS != NULL)
	{
		poSRS->Release();
	}
}

OGRFeature* OGRXODRLayer::GetNextFeature()
{
	switch (layerId)
	{
		case refLayer:
			return getReferenceLineFeatures();
			break;
		case laneLayer:
			return getLaneLineFeatures();
			break;
		case centerLanelayer:
			return  getCenterLaneLineFeatures();
			break;
		case signalLayer:
		 	return getSignalFeatures();
		 	break;
		case objectLayer:
			return getObjectFeatures();
			break;
		case markLayer:
			return getLaneLineFeatures();
			break;
		default:
			return getReferenceLineFeatures();
			break;
	}
}

OGRFeature* OGRXODRLayer::getReferenceLineFeatures()
{
	if (roadIter != simpleRoads.end())
	{
		RoadSF roadsf = *roadIter;
		OGRFeature *f = new OGRFeature(poFeatureDefn);
		OGRLineString *temp_l = new OGRLineString();
		std::unique_ptr<CoordinateSequence> cs = roadsf.getPlanViewSF()->getCoordinates();
		for(int i = 0; cs->getSize() > i ;i++){
            temp_l->setPoint(i,new OGRPoint(cs->getX(i),cs->getY(i)));
		}
        OGRGeometry *l = temp_l->MakeValid();
		f->SetGeometry(l);
		f->SetField(poFeatureDefn->GetFieldIndex("Name"), roadsf.getRoad().id().get().c_str()); //roadsf.getRoad().name().get().c_str()
		f->SetField(poFeatureDefn->GetFieldIndex("ID"), roadsf.getRoad().id().get().c_str());
		f->SetField(poFeatureDefn->GetFieldIndex("Junction"), roadsf.getRoad().junction().get().c_str());
		f->SetFID(nNextFID++);

		roadIter++;
		delete l;
		if ((m_poFilterGeom == NULL || FilterGeometry(f->GetGeometryRef()))
			&& (m_poAttrQuery == NULL || m_poAttrQuery->Evaluate(f)))
		{

			return f;
		}

		delete f;
	}
	return NULL;
}

OGRFeature* OGRXODRLayer::getCenterLaneLineFeatures()
{
	if (roadIter != simpleRoads.end())
	{
		if (featureIndex == 0)
		{
			try
			{
				currentCenterLanes = roadIter->getCenterLaneSf();
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}

		size_t nLanes = currentCenterLanes.size();
		OGRFeature* f = new OGRFeature(poFeatureDefn);

		if (nLanes > 0)
		{
			CenterLaneSF* lane = currentCenterLanes.at(featureIndex);
			OGRLineString *l = new OGRLineString();
			std::unique_ptr<CoordinateSequence> cs = lane->getGeometry()->getCoordinates();
			for(int i = 0; cs->getSize() > i ;i++){
				l->setPoint(i,new OGRPoint(cs->getX(i),cs->getY(i)));
			}

			f->SetGeometryDirectly(l->clone());
			f->SetField(poFeatureDefn->GetFieldIndex("ID"), (char)lane->getID());
			f->SetField(poFeatureDefn->GetFieldIndex("RoadID"), roadIter->getRoad().id().get().c_str());
			featureIndex++;
			delete l;
            delete lane;
		}

		if (featureIndex >= nLanes) {
			roadIter++;
			featureIndex = 0;
		}
		if ((m_poFilterGeom == NULL || FilterGeometry(f->GetGeometryRef()))
			&& (m_poAttrQuery == NULL || m_poAttrQuery->Evaluate(f)))
		{
			return f;
		}
		delete f;
	}
	return NULL;
}

OGRFeature* OGRXODRLayer::getLaneLineFeatures()
{
	if (roadIter != simpleRoads.end())
	{
		if (featureIndex == 0)
		{
			try
			{
				currentLanes = roadIter->getLanesSf();
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}


		}
		size_t nLanes = currentLanes.size();
		OGRFeature* f = new OGRFeature(poFeatureDefn);

		if (nLanes > 0)
		{
			LaneSF lane = currentLanes.at(featureIndex);
			OGRLineString *l = new OGRLineString();
			std::unique_ptr<CoordinateSequence> cs = lane.getGeometry()->getCoordinates();
			for(int i = 0; cs->getSize() > i ;i++){
				l->setPoint(i,new OGRPoint(cs->getX(i),cs->getY(i)));
			}

			f->SetGeometryDirectly(l->clone());
			f->SetField(poFeatureDefn->GetFieldIndex("ID"), (char)lane.GetRoadLane().id().get());
			f->SetField(poFeatureDefn->GetFieldIndex("Type"), lane.GetRoadLane().type().get().c_str());
			f->SetField(poFeatureDefn->GetFieldIndex("Level"), lane.GetRoadLane().level().get().c_str());
			f->SetField(poFeatureDefn->GetFieldIndex("RoadID"), roadIter->getRoad().id().get().c_str());
			featureIndex++;
			delete l;
		}

		if (featureIndex >= nLanes) {
			roadIter++;
			featureIndex = 0;
		}
		if ((m_poFilterGeom == NULL || FilterGeometry(f->GetGeometryRef()))
			&& (m_poAttrQuery == NULL || m_poAttrQuery->Evaluate(f)))
		{
			return f;
		}
		delete f;
	}
	return NULL;
}

//OGRFeature* OGRXODRLayer::getRoadMarkLineFeatures()
//{
//	return NULL;
//}

OGRFeature* OGRXODRLayer::getSignalFeatures()
{
	if(roadIter != simpleRoads.end())
	{
		if(featureIndex == 0){
		//find road with objects and skip empty roads
		 	while(roadIter != simpleRoads.end())
			{
				 try
				{
					currentSignals = roadIter->getSignalSf();
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				if(currentSignals.size() > 0){
		 			break;
		 		}
		 		roadIter++;
			}
		}
	int nFeatures =  currentSignals.size();

	OGRFeature* feature = new OGRFeature(poFeatureDefn);
	if(nFeatures > 0) {
	    SignalSF sigSf = currentSignals[featureIndex];
		OGRPoint *pFeature = new OGRPoint(sigSf.getP()->getX(), sigSf.getP()->getY());
	    feature->SetGeometryDirectly(pFeature);
		feature->SetField(poFeatureDefn->GetFieldIndex("ID"), sigSf.getXodrSignal().id().get().c_str());
        if (sigSf.getXodrSignal().name() != NULL)
            feature->SetField(poFeatureDefn->GetFieldIndex("Name"), sigSf.getXodrSignal().name().get().c_str());
		if (sigSf.getXodrSignal().type() != NULL)
            feature->SetField(poFeatureDefn->GetFieldIndex("Type"), sigSf.getXodrSignal().type().get().c_str());
		if (sigSf.getXodrSignal().subtype() != NULL)
            feature->SetField(poFeatureDefn->GetFieldIndex("Subtype"), sigSf.getXodrSignal().subtype().get().c_str());
		if (sigSf.getXodrSignal().dynamic() != NULL)
            feature->SetField(poFeatureDefn->GetFieldIndex("Dynamic"), sigSf.getXodrSignal().dynamic().get().c_str());
        if (sigSf.getXodrSignal().country() != NULL)
            feature->SetField(poFeatureDefn->GetFieldIndex("Country"), sigSf.getXodrSignal().country().get().c_str());

        feature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), roadIter->getRoad().id().get().c_str());
		if(sigSf.getXodrSignal().orientation() != NULL){
		    feature->SetField(poFeatureDefn->GetFieldIndex("Orient"), sigSf.getXodrSignal().orientation().get().c_str());
		}else{
		 	feature->SetField(poFeatureDefn->GetFieldIndex("Orient"), "");
		}

		featureIndex++;
	}
    if (featureIndex >= nFeatures && roadIter != simpleRoads.end())
    {
        roadIter++;
        featureIndex = 0;
    }
    if((m_poFilterGeom == NULL || FilterGeometry(feature->GetGeometryRef()))
        && (m_poAttrQuery == NULL || m_poAttrQuery->Evaluate(feature)))
        {
            return feature;
        }
		delete feature;
	}
	return NULL;
}

OGRFeature* OGRXODRLayer::getObjectFeatures()
{
	if(roadIter != simpleRoads.end())
	{
		if(featureIndex == 0){
		//find road with objects and skip empty roads
		 	while(roadIter != simpleRoads.end())
			{
				 try
				{
					currentObjects = roadIter->getObjectsSf();
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				if(currentObjects.size() > 0){
		 			break;
		 		}
		 		roadIter++;
			}
		}
        size_t nFeatures =  currentObjects.size();
		OGRFeature* feature = new OGRFeature(poFeatureDefn);
		if(nFeatures > 0) {

			ObjectSF objSf = currentObjects[featureIndex];

			OGRPoint *pFeature = new OGRPoint(objSf.getP()->getX(), objSf.getP()->getY());

			feature->SetGeometryDirectly(pFeature);
			if(objSf.getXodrObject().id() != NULL)
				feature->SetField(poFeatureDefn->GetFieldIndex("ID"), objSf.getXodrObject().id().get().c_str());

			if(objSf.getXodrObject().name() != NULL)
				feature->SetField(poFeatureDefn->GetFieldIndex("Name"), objSf.getXodrObject().name().get().c_str());

			if(objSf.getXodrObject().type() != NULL)
				feature->SetField(poFeatureDefn->GetFieldIndex("Type"), objSf.getXodrObject().type().get().c_str());

			feature->SetField(poFeatureDefn->GetFieldIndex("RoadID"), roadIter->getRoad().id().get().c_str());

			if(objSf.getXodrObject().orientation() != NULL){
				feature->SetField(poFeatureDefn->GetFieldIndex("Orient"), objSf.getXodrObject().orientation().get().c_str());
			}else{
				feature->SetField(poFeatureDefn->GetFieldIndex("Orient"), "");
			}


			featureIndex++;
		}
		if (featureIndex >= nFeatures && roadIter != simpleRoads.end())
		{
			roadIter++;
			featureIndex = 0;
		}
			if((m_poFilterGeom == NULL || FilterGeometry(feature->GetGeometryRef()))
				&& (m_poAttrQuery == NULL || m_poAttrQuery->Evaluate(feature)))
			{
				return feature;
			}
		delete feature;
	}
	return NULL;
}

void OGRXODRLayer::ResetReading()
{
	roadIter = simpleRoads.begin();
	nNextFID = 0;
}

void OGRXODRLayer::createRequestedLayer()
{
	switch (layerId)
	{
	case refLayer:
		{
			OGRFieldDefn oFieldName("Name", OFTString);
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldJunction("Junction", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldName);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldJunction);
			break;
		}
	case laneLayer:
		{
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldType("Type", OFTString);
			OGRFieldDefn oFieldLevel("Level", OFTString);
			OGRFieldDefn oFieldRoadID("RoadID", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldType);
			poFeatureDefn->AddFieldDefn(&oFieldLevel);
			poFeatureDefn->AddFieldDefn(&oFieldRoadID);
			break;
		}
	case centerLanelayer:
		{
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldRoadID("RoadID", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldRoadID);
			break;
		}
	case signalLayer:
		{
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldType("Type", OFTString);
			OGRFieldDefn oFieldName("Name", OFTString);
			OGRFieldDefn oFieldRoadID("RoadID", OFTString);
			OGRFieldDefn oFieldDynamic("Dynamic", OFTString);
			OGRFieldDefn oFieldCountry("Country", OFTString);
			OGRFieldDefn oFieldSubtype("Subtype", OFTString);
			OGRFieldDefn oFieldOrientation("Orient", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldRoadID);
			poFeatureDefn->AddFieldDefn(&oFieldName);
			poFeatureDefn->AddFieldDefn(&oFieldType);
			poFeatureDefn->AddFieldDefn(&oFieldOrientation);
			poFeatureDefn->AddFieldDefn(&oFieldDynamic);
			poFeatureDefn->AddFieldDefn(&oFieldCountry);
			poFeatureDefn->AddFieldDefn(&oFieldSubtype);
			break;
		}
	case objectLayer:
		{
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldName("Name", OFTString);
			// OGRFieldDefn oFieldRoll("Roll", OFTString);
			OGRFieldDefn oFieldType("Type", OFTString);
			// OGRFieldDefn oFieldPitch("Pitch", OFTString);
			// OGRFieldDefn oFieldRadius("Radius", OFTString);
			OGRFieldDefn oFieldRoadID("RoadID", OFTString);
			OGRFieldDefn oFieldOrientation("Orient", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldRoadID);
			poFeatureDefn->AddFieldDefn(&oFieldName);
			poFeatureDefn->AddFieldDefn(&oFieldType);
			poFeatureDefn->AddFieldDefn(&oFieldOrientation);
			// poFeatureDefn->AddFieldDefn(&oFieldPitch);
			// poFeatureDefn->AddFieldDefn(&oFieldRoll);
			// poFeatureDefn->AddFieldDefn(&oFieldRadius);
			break;
		}
	case markLayer:
		{
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldType("Type", OFTString);
			OGRFieldDefn oFieldRule("Rule", OFTString);
			OGRFieldDefn oFieldColor("Color", OFTString);
			OGRFieldDefn oFieldWeight("Weight", OFTString);
			OGRFieldDefn oFieldLaneChange("LaneChange", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldType);
			poFeatureDefn->AddFieldDefn(&oFieldRule);
			poFeatureDefn->AddFieldDefn(&oFieldColor);
			poFeatureDefn->AddFieldDefn(&oFieldWeight);
			poFeatureDefn->AddFieldDefn(&oFieldLaneChange);
			break;
		}
	default:
		{
			OGRFieldDefn oFieldName("Name", OFTString);
			OGRFieldDefn oFieldID("ID", OFTString);
			OGRFieldDefn oFieldJunction("Junction", OFTString);
			poFeatureDefn->AddFieldDefn(&oFieldName);
			poFeatureDefn->AddFieldDefn(&oFieldID);
			poFeatureDefn->AddFieldDefn(&oFieldJunction);
			break;
		}
	}
}
