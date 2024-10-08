/******************************************************************************
 *
 * Purpose:  Implementation of the CLinkSegment class.
 *
 ******************************************************************************
 * Copyright (c) 2009
 * PCI Geomatics, 90 Allstate Parkway, Markham, Ontario, Canada.
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "core/clinksegment.h"
#include "segment/cpcidsksegment.h"
#include "core/pcidsk_utils.h"
#include "pcidsk_exception.h"

#include <vector>
#include <string>
#include <cassert>
#include <cstring>

using namespace PCIDSK;

CLinkSegment::CLinkSegment(PCIDSKFile *fileIn,
                           int segmentIn,
                           const char *segment_pointer) :
    CPCIDSKSegment(fileIn, segmentIn, segment_pointer),
    loaded_(false), modified_(false)
{
    Load();
}


CLinkSegment::~CLinkSegment()
{
}

// Load the contents of the segment
void CLinkSegment::Load()
{
    // Check if we've already loaded the segment into memory
    if (loaded_) {
        return;
    }

    seg_data.SetSize(data_size < 1024 ? -1 : (int) (data_size - 1024));

    ReadFromFile(seg_data.buffer, 0, data_size - 1024);

    if (seg_data.buffer_size < 8)
    {
        path.clear();
        return;
    }

    if (!STARTS_WITH(seg_data.buffer, "SysLinkF"))
    {
        seg_data.Put("SysLinkF",0,8);
        return;
    }

    const char * pszEnd = seg_data.buffer + seg_data.buffer_size;
    const char * pszPathStart = seg_data.buffer + 8;
    const char * pszPathEnd = pszPathStart;

    // Find the end of the path.
    while (pszPathEnd < pszEnd && *pszPathEnd)
        ++pszPathEnd;

    // Remove trailing spaces.
    while (pszPathEnd > pszPathStart && *pszPathEnd == ' ')
        --pszPathEnd;

    path = std::string(pszPathStart, pszPathEnd);

    // We've now loaded the structure up with data. Mark it as being loaded
    // properly.
    loaded_ = true;

}

void CLinkSegment::Write(void)
{
    //We are not writing if nothing was loaded.
    if (!modified_) {
        return;
    }

    seg_data.Put("SysLinkF",0,8);
    seg_data.Put(path.c_str(), 8, static_cast<int>(path.size()), true);

    WriteToFile(seg_data.buffer, 0, data_size-1024);
    modified_ = false;
}

std::string CLinkSegment::GetPath(void) const
{
    return path;
}

void CLinkSegment::SetPath(const std::string& oPath)
{
    if(oPath.size() < 504)
    {
        path = oPath;
        modified_ = true;
    }
    else
    {
        return ThrowPCIDSKException("The size of the path cannot be"
                              " bigger than 504 characters.");
    }
}

void CLinkSegment::Synchronize()
{
    if(modified_)
    {
        this->Write();
    }
}

