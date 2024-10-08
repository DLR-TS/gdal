/******************************************************************************
 *
 * Purpose:  Implementation of pthreads based mutex.
 *
 ******************************************************************************
 * Copyright (c) 2009
 * PCI Geomatics, 90 Allstate Parkway, Markham, Ontario, Canada.
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "pcidsk_mutex.h"
#include <pthread.h>

#include <cstdlib>


/************************************************************************/
/*                             PThreadMutex                             */
/************************************************************************/

class PThreadMutex : public PCIDSK::Mutex

{
private:
    pthread_mutex_t *hMutex;

    PThreadMutex(const PThreadMutex&) = delete;
    PThreadMutex& operator=(const PThreadMutex&) = delete;

public:
    PThreadMutex();
    ~PThreadMutex();

    int Acquire(void) override;
    int Release(void) override;
};

/************************************************************************/
/*                            PThreadMutex()                            */
/************************************************************************/

PThreadMutex::PThreadMutex()

{
    hMutex = static_cast<pthread_mutex_t *>(malloc(sizeof(pthread_mutex_t)));

#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
    pthread_mutex_t tmp_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    *hMutex = tmp_mutex;
#elif defined(PTHREAD_MUTEX_RECURSIVE) || defined(HAVE_PTHREAD_MUTEX_RECURSIVE)
    {
        pthread_mutexattr_t  attr;
        pthread_mutexattr_init( &attr );
        pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
        pthread_mutex_init( hMutex, &attr );
    }
#else
#error "Recursive mutexes apparently unsupported, configure --without-threads"
#endif
}

/************************************************************************/
/*                           ~PThreadMutex()                            */
/************************************************************************/

PThreadMutex::~PThreadMutex()

{
    pthread_mutex_destroy( hMutex );
    free( hMutex );
}

/************************************************************************/
/*                              Release()                               */
/************************************************************************/

int PThreadMutex::Release()

{
    pthread_mutex_unlock( hMutex );
    return 1;
}

/************************************************************************/
/*                              Acquire()                               */
/************************************************************************/

int PThreadMutex::Acquire()

{
    return pthread_mutex_lock( hMutex ) == 0;
}

/************************************************************************/
/*                         DefaultCreateMutex()                         */
/************************************************************************/

/**
 * Create a mutex.
 *
 * This function creates the default style of mutex for the currently
 * PCIDSK library build.  The mutex should be destroyed with delete when
 * no longer required.
 *
 * @return a new mutex object pointer.
 */


PCIDSK::Mutex *PCIDSK::DefaultCreateMutex(void)

{
    return new PThreadMutex();
}
