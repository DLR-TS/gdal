/******************************************************************************
 *
 * Project:  GDAL
 * Purpose:  Private header
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef OGR_PROJ_P_H_INCLUDED
#define OGR_PROJ_P_H_INCLUDED

#include "proj.h"

#include "cpl_mem_cache.h"

#include <unordered_map>
#include <memory>
#include <utility>

/*! @cond Doxygen_Suppress */

PJ_CONTEXT CPL_DLL *OSRGetProjTLSContext();
void OSRCleanupTLSContext();

class OSRProjTLSCache
{
    struct OSRPJDeleter
    {
        void operator()(PJ *pj) const
        {
            proj_destroy(pj);
        }
    };

    typedef std::unique_ptr<PJ, OSRPJDeleter> UniquePtrPJ;

    struct EPSGCacheKey
    {
        int nCode_;
        bool bUseNonDeprecated_;
        bool bAddTOWGS84_;

        EPSGCacheKey(int nCode, bool bUseNonDeprecated, bool bAddTOWGS84)
            : nCode_(nCode), bUseNonDeprecated_(bUseNonDeprecated),
              bAddTOWGS84_(bAddTOWGS84)
        {
        }

        bool operator==(const EPSGCacheKey &other) const
        {
            return nCode_ == other.nCode_ &&
                   bUseNonDeprecated_ == other.bUseNonDeprecated_ &&
                   bAddTOWGS84_ == other.bAddTOWGS84_;
        }
    };

    struct EPSGCacheKeyHasher
    {
        std::size_t operator()(const EPSGCacheKey &k) const
        {
            return k.nCode_ | ((k.bUseNonDeprecated_ ? 1 : 0) << 16) |
                   ((k.bAddTOWGS84_ ? 1 : 0) << 17);
        }
    };

    PJ_CONTEXT *m_tlsContext =
        nullptr;  // never use it directly. use GetPJContext()
    lru11::Cache<EPSGCacheKey, UniquePtrPJ, lru11::NullLock,
                 std::unordered_map<EPSGCacheKey,
                                    typename std::list<lru11::KeyValuePair<
                                        EPSGCacheKey, UniquePtrPJ>>::iterator,
                                    EPSGCacheKeyHasher>>
        m_oCacheEPSG{};
    lru11::Cache<std::string, UniquePtrPJ> m_oCacheWKT{};

    PJ_CONTEXT *GetPJContext();

    OSRProjTLSCache(const OSRProjTLSCache &) = delete;
    OSRProjTLSCache &operator=(const OSRProjTLSCache &) = delete;

  public:
    explicit OSRProjTLSCache(PJ_CONTEXT *tlsContext) : m_tlsContext(tlsContext)
    {
    }

    void clear();

    PJ *GetPJForEPSGCode(int nCode, bool bUseNonDeprecated, bool bAddTOWGS84);
    void CachePJForEPSGCode(int nCode, bool bUseNonDeprecated, bool bAddTOWGS84,
                            PJ *pj);

    PJ *GetPJForWKT(const std::string &wkt);
    void CachePJForWKT(const std::string &wkt, PJ *pj);
};

OSRProjTLSCache *OSRGetProjTLSCache();

void OGRCTDumpStatistics();

void OSRCTCleanCache();

/*! @endcond Doxygen_Suppress */

#endif
