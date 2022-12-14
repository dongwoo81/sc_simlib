/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2005 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_cor_pthread.cpp -- Coroutine implementation with pthreads.

  Original Author: Andy Goodrich, Forte Design Systems, 2002-11-10

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#if defined(SC_USE_PTHREADS)

// ORDER OF THE INCLUDES AND namespace sc_core IS IMPORTANT!!!

using namespace std;

#include "sysc/kernel/sc_cor_pthread.h"
#include "sysc/kernel/sc_simcontext.h"

namespace sc_core {

// MAKE SURE WE HAVE A NULL THAT WILL WORK:

#if defined(__hpux)
#   define PTHREAD_NULL cma_c_null
#else  // !defined(__hpux)
#   define PTHREAD_NULL NULL
#endif // !defined(__hpux)

#define DEBUGF if (0) std::cout

// ----------------------------------------------------------------------------
//  File static variables.
//
// (1) The thread creation mutex and the creation condition are used to
//     suspend the thread creating another one until the created thread
//     reaches its invoke_module_method. This allows us to get control of
//     thread scheduling away from the pthread package.
// ----------------------------------------------------------------------------

static sc_cor_pthread* active_cor_p=0;   // Active co-routine.
static pthread_cond_t  create_condition; // See note 1 above.
static pthread_mutex_t create_mutex;     // See note 1 above.
static sc_cor_pthread  main_cor;         // Main coroutine.


// ----------------------------------------------------------------------------
//  CLASS : sc_cor_pthread
//
//  Coroutine class implemented with Posix Threads.
// ----------------------------------------------------------------------------

// constructor

sc_cor_pthread::sc_cor_pthread()
    : m_cor_fn_arg( 0 ), m_pkg_p( 0 )
{
    DEBUGF << this << ": sc_cor_pthread::sc_cor_pthread()" << std::endl;
    pthread_cond_init( &m_pt_condition, PTHREAD_NULL );
    pthread_mutex_init( &m_mutex, PTHREAD_NULL );
}


// destructor

sc_cor_pthread::~sc_cor_pthread()
{
}


// This static method is a Posix Threads helper callback and invokes a thread
// for the first time. It performs some synchronization and then invokes the
// actual sc_cor helper function.
//     context_p -> thread to invoke module method of.
// Result is 0 and ignored.

void* sc_cor_pthread::invoke_module_method(void* context_p)
{
    sc_cor_pthread* p = (sc_cor_pthread*)context_p;
    DEBUGF << p << ": sc_cor_pthread::invoke_module_method()" << std::endl;


    // SUSPEND THE THREAD SO WE CAN GAIN CONTROL FROM THE PTHREAD PACKAGE:
    //
    // Since pthread_create schedules each thread behind our back for its
    // initial execution we immediately suspend a newly created thread
    // here so we can control when its execution will occur. We also wake
    // up the main thread which is waiting for this thread to execute to this
    // wait point.

    pthread_mutex_lock( &create_mutex );
    pthread_cond_signal( &create_condition );
    pthread_mutex_lock( &p->m_mutex );
    pthread_mutex_unlock( &create_mutex );
    // DEBUGF("%08x: wait %d\n", (int)p, __LINE__);
    pthread_cond_wait( &p->m_pt_condition, &p->m_mutex );
    // DEBUGF("%08x: back from wait %d\n", (int)p, __LINE__);
    pthread_mutex_unlock( &p->m_mutex );


    // CALL THE SYSTEMC CODE THAT WILL ACTUALLY START THE THREAD OFF:

    DEBUGF << p << ": about to invoke real method <<  active_cor_p"<< std::endl;
    active_cor_p = p;
    (p->m_cor_fn)(p->m_cor_fn_arg);

    return 0;
}


// ----------------------------------------------------------------------------
//  CLASS : sc_cor_pkg_pthread
//
//  Coroutine package class implemented with Posix Threads.
// ----------------------------------------------------------------------------

int sc_cor_pkg_pthread::instance_count = 0;


// constructor

sc_cor_pkg_pthread::sc_cor_pkg_pthread( sc_simcontext* simc )
: sc_cor_pkg( simc )
{
    // initialize the current coroutine
    if( ++ instance_count == 1 )
    {
        pthread_cond_init( &create_condition, PTHREAD_NULL );
        pthread_mutex_init( &create_mutex, PTHREAD_NULL );
        assert( active_cor_p == 0 );
        main_cor.m_pkg_p = this;
        active_cor_p = &main_cor;
    }
}


// destructor

sc_cor_pkg_pthread::~sc_cor_pkg_pthread()
{
    if( -- instance_count == 0 ) {
        // cleanup the main coroutine
    }
}


// create a new coroutine

sc_cor*
sc_cor_pkg_pthread::create( std::size_t stack_size, sc_cor_fn* fn, void* arg )
{
    sc_cor_pthread* cor_p = new sc_cor_pthread;
    DEBUGF << cor_p << ": sc_cor_pkg_pthread::create()" << std::endl;


    // INITIALIZE OBJECT'S FIELDS FROM ARGUMENT LIST:

    cor_p->m_pkg_p = this;
    cor_p->m_cor_fn = fn;
    cor_p->m_cor_fn_arg = arg;


    // ALLOCATE THE POSIX THREAD TO USE AND FORCE SEQUENTIAL EXECUTION:
    //
    // Because pthread_create causes the created thread to be executed,
    // we need to let it run until we can block in the invoke_module_method.
    // So we:
    //   (1) Lock the creation mutex before creating the new thread.
    //   (2) Sleep on the creation condition, which will be signalled by
    //       the newly created thread just before it goes to sleep in
    //       invoke_module_method.
    // This scheme results in the newly created thread being dormant before
    // the main thread continues execution.

    pthread_mutex_lock( &create_mutex );
    if ( pthread_create( &cor_p->m_thread, PTHREAD_NULL,
             &sc_cor_pthread::invoke_module_method, (void*)cor_p ) )
    {
        fprintf(stderr, "ERROR - could not create thread\n");
    }

    DEBUGF << cor_p << ": wait() " << __LINE__ << std::endl;
    pthread_cond_wait( &create_condition, &create_mutex );
    DEBUGF << cor_p << ": back from wait() " << __LINE__ << std::endl;
    pthread_mutex_unlock( &create_mutex );
    DEBUGF << cor_p << ": exiting sc_cor_pkg_pthread::create() "
	       << __LINE__ << std::endl;

    return cor_p;
}


// yield to the next coroutine
//
// We don't do anything after the p_thread_cond_wait since it won't
// happen until the thread wakes up again!

void
sc_cor_pkg_pthread::yield( sc_cor* next_cor_p )
{
    sc_cor_pthread* from_p = active_cor_p;
    sc_cor_pthread* to_p = (sc_cor_pthread*)next_cor_p;

    DEBUGF << from_p << ": switch to " << to_p << std::endl;
    if ( to_p != from_p )
    {
        pthread_mutex_lock( &to_p->m_mutex );
        pthread_cond_signal( &to_p->m_pt_condition );
        pthread_mutex_lock( &from_p->m_mutex );
        pthread_mutex_unlock( &to_p->m_mutex );
		DEBUGF << from_p << ": wait " << __LINE__ << " "
		       << &from_p << std::endl;
        pthread_cond_wait( &from_p->m_pt_condition, &from_p->m_mutex );
		DEBUGF << from_p << ": back from wait " << __LINE__ << " "
		       << &from_p << std::endl;
        pthread_mutex_unlock( &from_p->m_mutex );
    }

    active_cor_p = from_p; // When we come out of wait restore ourselves as active.
}


// abort the current coroutine (and resume the next coroutine)

void
sc_cor_pkg_pthread::abort( sc_cor* next_cor_p )
{
    sc_cor_pthread* n_p = (sc_cor_pthread*)next_cor_p;

    DEBUGF << active_cor_p << ": aborting, switching to " << n_p << std::endl;
    pthread_mutex_lock( &n_p->m_mutex );
    pthread_cond_signal( &n_p->m_pt_condition );
    pthread_mutex_unlock( &n_p->m_mutex );

    active_cor_p = n_p;
}


// get the main coroutine

sc_cor*
sc_cor_pkg_pthread::get_main()
{
    return &main_cor;
}

} // namespace sc_core

#endif // defined(SC_USE_PTHREADS)


// Taf!
