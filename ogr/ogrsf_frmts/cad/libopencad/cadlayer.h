/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *  Copyright (c) 2016 NextGIS, <info@nextgis.com>
 *
  * SPDX-License-Identifier: MIT
 *******************************************************************************/
#ifndef CADLAYER_H
#define CADLAYER_H

#include "cadgeometry.h"

#include <memory>
#include <unordered_set>

class CADFile;

class OCAD_EXTERN CADLayer
{
public:
    explicit       CADLayer( CADFile * file );
    std::string getName() const;
    void   setName( const std::string& value );

    bool getFrozen() const;
    void setFrozen( bool value );

    bool getOn() const;
    void setOn( bool value );

    bool getFrozenByDefault() const;
    void setFrozenByDefault( bool value );

    bool getLocked() const;
    void setLocked( bool value );

    bool getPlotting() const;
    void setPlotting( bool value );

    short getLineWeight() const;
    void  setLineWeight( short value );

    short getColor() const;
    void  setColor( short value );

    size_t getId() const;
    void   setId( const size_t& value );

    long getHandle() const;
    void setHandle( long value );

    std::unordered_set<std::string> getAttributesTags();

    // cadinserthandle is 0 by default because if entity isn't a part of custom block - its a part of ModelSpace block.
    void addHandle( long handle, enum CADObject::ObjectType type, long cadinserthandle = 0 );

    size_t getGeometryCount() const;
    CADGeometry * getGeometry( size_t index );
    size_t getImageCount() const;
    CADImage * getImage( size_t index );

    /**
     * @brief returns a vector of presented geometries types
     */
    std::vector<CADObject::ObjectType> getGeometryTypes();

protected:
    bool addAttribute( const CADObject * pObject );
protected:
    std::string layerName;
    bool   frozen;
    bool   on;
    bool   frozenByDefault;
    bool   locked;
    bool   plotting;
    short  lineWeight;
    short  color;
    size_t layerId;
    long   layer_handle;

    std::vector<CADObject::ObjectType>  geometryTypes; // FIXME: replace with hashset would be perfect
    std::unordered_set<std::string>     attributesNames;
    std::vector<std::pair<long, long> > geometryHandles; // second param is CADInsert handle, 0 if its not a geometry in block ref.
    std::vector<long>                   imageHandles;
    std::vector<std::pair<long, std::map<std::string, long> > > geometryAttributes;
    std::map<long, Matrix>              transformations;

    CADFile * pCADFile;
};

#endif // CADLAYER_H
