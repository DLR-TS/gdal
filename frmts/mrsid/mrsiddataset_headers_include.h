/******************************************************************************
 *
 * Project:  Multi-resolution Seamless Image Database (MrSID)
 * Purpose:  Read/write LizardTech's MrSID file format - Version 4+ SDK.
 * Author:   Andrey Kiselev, dron@ak4719.spb.edu
 *
 ******************************************************************************
 * Copyright (c) 2003, Andrey Kiselev <dron@ak4719.spb.edu>
 * Copyright (c) 2007-2013, Even Rouault <even dot rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef MRSIDDATASET_HEADERS_INCLUDE_H
#define MRSIDDATASET_HEADERS_INCLUDE_H

#include "cpl_port.h"

#ifdef HAVE_GCC_SYSTEM_HEADER
#pragma GCC system_header
#endif

// Key Macros from Makefile:
//   MRSID_ESDK: Means we have the encoding SDK (version 5 or newer required)
//   MRSID_J2K: Means we are enabling MrSID SDK JPEG2000 support.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wdocumentation"
#endif

#include "lt_types.h"
#include "lt_base.h"
#include "lt_fileSpec.h"
#include "lt_ioFileStream.h"
#include "lt_utilStatusStrings.h"
#include "lti_geoCoord.h"
#include "lti_pixel.h"
#include "lti_navigator.h"
#include "lti_sceneBuffer.h"
#include "lti_metadataDatabase.h"
#include "lti_metadataRecord.h"
#include "lti_utils.h"
#include "lti_delegates.h"
#include "lt_utilStatus.h"
#include "MrSIDImageReader.h"

#ifdef MRSID_J2K
#include "J2KImageReader.h"
#endif

// It seems that LT_STS_UTIL_TimeUnknown was added in version 6, also
// the first version with lti_version.h
#ifdef LT_STS_UTIL_TimeUnknown
#include "lti_version.h"
#endif

// Are we using version 6 or newer?
#if defined(LTI_SDK_MAJOR) && LTI_SDK_MAJOR >= 6
#define MRSID_POST5
#endif

#ifdef MRSID_ESDK
#include "MG3ImageWriter.h"
#include "MG3WriterParams.h"
#include "MG2ImageWriter.h"
#include "MG2WriterParams.h"
#ifdef MRSID_HAVE_MG4WRITE
#include "MG4ImageWriter.h"
#include "MG4WriterParams.h"
#endif
#ifdef MRSID_J2K
#ifdef MRSID_POST5
#include "JP2WriterManager.h"
#include "JPCWriterParams.h"
#else
#include "J2KImageWriter.h"
#include "J2KWriterParams.h"
#endif
#endif
#endif /* MRSID_ESDK */

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif  // MRSIDDATASET_HEADERS_INCLUDE_H
