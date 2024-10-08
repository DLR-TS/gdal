/******************************************************************************
 *
 * Purpose:  Implementation of the PCIDSKBuffer class.  This class is for
 *           convenient parsing and formatting of PCIDSK ASCII headers.
 *
 ******************************************************************************
 * Copyright (c) 2009
 * PCI Geomatics, 90 Allstate Parkway, Markham, Ontario, Canada.
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "pcidsk_config.h"
#include "pcidsk_buffer.h"
#include "pcidsk_exception.h"
#include "core/pcidsk_utils.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sstream>

using namespace PCIDSK;

/************************************************************************/
/*                            PCIDSKBuffer()                            */
/************************************************************************/

PCIDSKBuffer::PCIDSKBuffer( int size )

{
    buffer_size = 0;
    buffer = nullptr;

    if( size > 0 )
        SetSize( size );
}

/************************************************************************/
/*                            PCIDSKBuffer()                            */
/************************************************************************/

PCIDSKBuffer::PCIDSKBuffer( const char *src, int size )

{
    buffer_size = 0;
    buffer = nullptr;

    SetSize( size );
    memcpy( buffer, src, size );
}

/************************************************************************/
/*                           ~PCIDSKBuffer()                            */
/************************************************************************/

PCIDSKBuffer::~PCIDSKBuffer()

{
    free( buffer );
}

/************************************************************************/
/*                              SetSize()                               */
/************************************************************************/

void PCIDSKBuffer::SetSize( int size )

{
    if( size < 0 )
    {
        free( buffer );
        buffer = nullptr;
        buffer_size = 0;
        throw PCIDSKException( "Invalid buffer size: %d", size );
    }
    buffer_size = size;
    char* new_buffer = (char *) realloc(buffer,size+1);

    if( new_buffer == nullptr )
    {
        free( buffer );
        buffer = nullptr;
        buffer_size = 0;
        throw PCIDSKException( "Out of memory allocating %d byte PCIDSKBuffer.",
                               size );
    }

    buffer = new_buffer;
    buffer[size] = '\0';
}

/************************************************************************/
/*                                Get()                                 */
/************************************************************************/

const char *PCIDSKBuffer::Get( int offset, int size ) const

{
    Get( offset, size, work_field, 0 );
    return work_field.c_str();
}

/************************************************************************/
/*                                Get()                                 */
/************************************************************************/

void PCIDSKBuffer::Get( int offset, int size, std::string &target, int unpad ) const

{
    if( offset + size > buffer_size )
        return ThrowPCIDSKException( "Get() past end of PCIDSKBuffer." );

    if( unpad )
    {
        while( size > 0 && buffer[offset+size-1] == ' ' )
            size--;
    }

    target.assign( buffer + offset, size );
}

/************************************************************************/
/*                             GetUInt64()                              */
/************************************************************************/

uint64 PCIDSKBuffer::GetUInt64( int offset, int size ) const

{
    std::string value_str;

    if( offset + size > buffer_size )
        return ThrowPCIDSKException(0, "GetUInt64() past end of PCIDSKBuffer." );

    value_str.assign( buffer + offset, size );

    return atouint64(value_str.c_str());
}

/************************************************************************/
/*                               GetInt()                               */
/************************************************************************/

int PCIDSKBuffer::GetInt( int offset, int size ) const

{
    std::string value_str;

    if( offset + size > buffer_size )
        return ThrowPCIDSKException(0, "GetInt() past end of PCIDSKBuffer." );

    value_str.assign( buffer + offset, size );

    return atoi(value_str.c_str());
}

/************************************************************************/
/*                             GetDouble()                              */
/************************************************************************/

double PCIDSKBuffer::GetDouble( int offset, int size ) const

{
    std::string value_str;

    if( offset + size > buffer_size )
        return ThrowPCIDSKException(0, "GetDouble() past end of PCIDSKBuffer." );

    value_str.assign( buffer + offset, size );

/* -------------------------------------------------------------------- */
/*      PCIDSK uses FORTRAN 'D' format for doubles - convert to 'E'     */
/*      (C style) before calling CPLAtof.                                  */
/* -------------------------------------------------------------------- */
    int i;

    for( i = 0; i < size; i++ )
    {
        if( value_str[i] == 'D' )
            value_str[i] = 'E';
    }

    return CPLAtof(value_str.c_str());
}

/************************************************************************/
/*                                Put()                                 */
/************************************************************************/

void PCIDSKBuffer::Put( const char *value, int offset, int size, bool null_term )

{
    if( offset + size > buffer_size )
        return ThrowPCIDSKException( "Put() past end of PCIDSKBuffer." );

    int v_size = static_cast<int>(strlen(value));
    if( v_size > size )
        v_size = size;

    if( v_size < size )
        memset( buffer + offset, ' ', size );

    memcpy( buffer + offset, value, v_size );

    if (null_term)
    {
        *(buffer + offset + v_size) = '\0';
    }
}

/************************************************************************/
/*                            PutBin(double)                            */
/************************************************************************/

void PCIDSKBuffer::PutBin(double value, int offset)
{
    const char* pszValue = (const char*)&value;
    memcpy( buffer + offset, pszValue, 8 );
}

/************************************************************************/
/*                              PutBin(int16)                           */
/************************************************************************/

void PCIDSKBuffer::PutBin(int16 value, int offset)
{
    const char * pszValue = (const char *) &value;

    memcpy(buffer + offset, pszValue, sizeof(int16));
}

/************************************************************************/
/*                             Put(uint64)                              */
/************************************************************************/

void PCIDSKBuffer::Put( uint64 value, int offset, int size )

{
    char fmt[64];
    char wrk[128];

    snprintf( fmt, sizeof(fmt), "%%%d%sd", size, PCIDSK_FRMT_64_WITHOUT_PREFIX );
    snprintf( wrk, sizeof(wrk), fmt, value );

    Put( wrk, offset, size );
}

/************************************************************************/
/*                             Put(double)                              */
/************************************************************************/

void PCIDSKBuffer::Put( double value, int offset, int size,
                        const char *fmt )

{
    if( fmt == nullptr )
        fmt = "%g";

    char wrk[128];
    CPLsnprintf( wrk, 127, fmt, value );

    char *exponent = strstr(wrk,"E");
    if( exponent != nullptr )
        *exponent = 'D';

    Put( wrk, offset, size );
}

/************************************************************************/
/*                             operator=()                              */
/************************************************************************/

PCIDSKBuffer &PCIDSKBuffer::operator=( const PCIDSKBuffer &src )

{
    if( this != &src )
    {
        SetSize( src.buffer_size );
        memcpy( buffer, src.buffer, buffer_size );
    }

    return *this;
}
