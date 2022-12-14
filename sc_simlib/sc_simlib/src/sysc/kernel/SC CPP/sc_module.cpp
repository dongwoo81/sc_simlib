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

  sc_module.cpp -- Base class of all sequential and combinational processes.

  Original Author: Stan Y. Liao, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Ali Dasdan, Synopsys, Inc.
  Description of Modification: - Implementation of operator() and operator,
                                 positional connection method.
                               - Implementation of error checking in
                                 operator<<'s.
                               - Implementation of the function test_module_prm.
                               - Implementation of set_stack_size().

      Name, Affiliation, Date: Andy Goodrich, Forte Design Systems 20 May 2003
  Description of Modification: Inherit from sc_process_host

      Name, Affiliation, Date: Bishnupriya Bhattacharya, Cadence Design Systems,
                               25 August, 2003
  Description of Modification: dont_initialize() uses 
                               sc_get_last_created_process_handle() instead of
                               sc_get_curr_process_handle()

      Name, Affiliation, Date: Andy Goodrich, Forte Design Systems 25 Mar 2003
  Description of Modification: Fixed bug in SC_NEW, see comments on 
                               ~sc_module_dynalloc_list below.


 *****************************************************************************/


#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_kernel_ids.h"
#include "sysc/kernel/sc_macros_int.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_name_gen.h"
#include "sysc/kernel/sc_object_manager.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_process_int.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_simcontext_int.h"
#include "sysc/communication/sc_communication_ids.h"
#include "sysc/communication/sc_interface.h"
#include "sysc/communication/sc_port.h"
#include "sysc/communication/sc_signal.h"
#include "sysc/communication/sc_signal_ports.h"
#include "sysc/utils/sc_iostream.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_module_dynalloc_list
//
//  Garbage collection for modules dynamically allocated with SC_NEW.
// ----------------------------------------------------------------------------

class sc_module_dynalloc_list
{
public:

    sc_module_dynalloc_list()
        {}

    ~sc_module_dynalloc_list();

    void add( sc_module* p )
        { m_list.push_back( p ); }

private:

    sc_plist<sc_module*> m_list;
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
// Note we clear the m_parent field for the module being deleted. This because
// we process the list front to back so the parent has already been deleted, 
// and we don't want ~sc_object() to try to access the parent which may 
// contain garbage.

sc_module_dynalloc_list::~sc_module_dynalloc_list()
{
    sc_plist<sc_module*>::iterator it( m_list );
    while( ! it.empty() ) {
        (*it)->m_parent = 0;
        delete *it;
        it ++;
    }
}


// ----------------------------------------------------------------------------

sc_module*
sc_module_dynalloc( sc_module* module_ )
{
    static sc_module_dynalloc_list dynalloc_list;
    dynalloc_list.add( module_ );
    return module_;
}


// ----------------------------------------------------------------------------
//  STRUCT : sc_bind_proxy
//
//  Struct for temporarily storing a pointer to an interface or port.
//  Used for positional binding.
// ----------------------------------------------------------------------------
    
sc_bind_proxy::sc_bind_proxy()
: iface( 0 ),
  port( 0 )
{}

sc_bind_proxy::sc_bind_proxy( sc_interface& iface_ )
: iface( &iface_ ),
  port( 0 )
{}

sc_bind_proxy::sc_bind_proxy( sc_port_base& port_ )
: iface( 0 ),
  port( &port_ )
{}


const sc_bind_proxy SC_BIND_PROXY_NIL;


// ----------------------------------------------------------------------------
//  CLASS : sc_module
//
//  Base class for all structural entities.
// ----------------------------------------------------------------------------

void
sc_module::sc_module_init()
{
    simcontext()->hierarchy_push( this );
    m_end_module_called = false;
    m_port_vec = new sc_pvector<sc_port_base*>( 16 );
    m_port_index = 0;
    m_name_gen = new sc_name_gen;
    simcontext()->get_module_registry()->insert( *this );
}

sc_module::sc_module( const char* nm )
: sc_process_host(nm),
  sensitive(this),
  sensitive_pos(this),
  sensitive_neg(this)
{
    sc_module_init();
}

/*
 *  This form of the constructor assumes that the user has
 *  used an sc_module_name parameter for his/her constructor.
 *  That parameter object has been pushed onto the stack,
 *  and can be looked up by calling the 
 *  top_of_module_name_stack() function of the object manager.
 *  This technique has two advantages:
 *
 *  1) The user no longer has to write sc_module(name) in the
 *     constructor initializer.
 *  2) The user no longer has to call end_module() at the end
 *     of the constructor -- a common negligence.
 *
 *  But it is important to note that sc_module_name may be used
 *  in the user's constructor's parameter. If it is used anywhere
 *  else, unexpected things will happen. The error-checking
 *  mechanism builtin here cannot hope to catch all misuses.
 *
 */

sc_module::sc_module()
: sc_process_host(::sc_core::sc_get_curr_simcontext()
                  ->get_object_manager()
                  ->top_of_module_name_stack()
                  ->operator const char*()),
  sensitive(this),
  sensitive_pos(this),
  sensitive_neg(this)
{
    /* When this form is used, we better have a fresh sc_module_name
       on the top of the stack */
    sc_module_name* mod_name = 
        simcontext()->get_object_manager()->top_of_module_name_stack();
    if (0 == mod_name || 0 != mod_name->m_module)
        SC_REPORT_ERROR( SC_ID_SC_MODULE_NAME_REQUIRED_, 0 );
    mod_name->m_module = this;
    sc_module_init();
}

sc_module::sc_module( const sc_module_name& )
: sc_process_host(::sc_core::sc_get_curr_simcontext()
                  ->get_object_manager()
                  ->top_of_module_name_stack()
                  ->operator const char*()),
  sensitive(this),
  sensitive_pos(this),
  sensitive_neg(this)
{
    /* For those used to the old style of passing a name to sc_module,
       this constructor will reduce the chance of making a mistake */

    /* When this form is used, we better have a fresh sc_module_name
       on the top of the stack */
    sc_module_name* mod_name = 
        simcontext()->get_object_manager()->top_of_module_name_stack();
    if (0 == mod_name || 0 != mod_name->m_module)
      SC_REPORT_ERROR( SC_ID_SC_MODULE_NAME_REQUIRED_, 0 );
    mod_name->m_module = this;
    sc_module_init();
}

sc_module::sc_module( const std::string& s )
: sc_process_host( s.c_str() ),
  sensitive(this),
  sensitive_pos(this),
  sensitive_neg(this)
{
    sc_module_init();
}

sc_module::~sc_module()
{
    delete m_port_vec;
    delete m_name_gen;
    simcontext()->get_module_registry()->remove( *this );
}


const ::std::vector<sc_object*>&
sc_module::get_child_objects() const
{
    return m_child_objects;
}

void
sc_module::add_child_object( sc_object* object_ )
{
    // no check if object_ is already in the set
    m_child_objects.push_back( object_ );
}

void
sc_module::remove_child_object( sc_object* object_ )
{
    int size = m_child_objects.size();
    for( int i = 0; i < size; ++ i ) {
	if( object_ == m_child_objects[i] ) {
	    m_child_objects[i] = m_child_objects[size - 1];
	    m_child_objects.decr_count();
	    return;
	}
    }
    // no check if object_ is really in the set
}


void
sc_module::end_module()
{
    if( ! m_end_module_called ) {
	/* TBD: Can check here to alert the user that end_module
                was not called for a previous module. */
	(void) simcontext()->hierarchy_pop();
	simcontext()->reset_curr_proc(); 
	sensitive.reset();
	sensitive_pos.reset();
	sensitive_neg.reset();
	m_end_module_called = true;
    }
}


// to prevent initialization for SC_METHODs and SC_THREADs

void
sc_module::dont_initialize()
{
    sc_process_b* last_proc = sc_get_last_created_process_handle();
    if (!last_proc) return; 
    if (last_proc->is_cthread()) {
        SC_REPORT_WARNING( SC_ID_DONT_INITIALIZE_, 0 );
    } else {
	last_proc->do_initialize( false );
    }
}

// set SC_CTHREAD reset sensitivity

void
sc_module::reset_signal_is( const sc_in<bool>& port, bool level )
{
	watching( port.delayed() == level );
}

void
sc_module::reset_signal_is( const sc_signal<bool>& sig, bool level )
{
	watching( sig.delayed() == level );
}

// to generate unique names for objects in an MT-Safe way

const char*
sc_module::gen_unique_name( const char* basename_, bool preserve_first )
{
    return m_name_gen->gen_unique_name( basename_, preserve_first );
}


// called by construction_done 

void
sc_module::before_end_of_elaboration()
{}

void
sc_module::construction_done()
{
    before_end_of_elaboration();
}

// called by elaboration_done (does nothing by default)

void
sc_module::end_of_elaboration()
{}


void
sc_module::elaboration_done( bool& error_ )
{
    if( ! m_end_module_called ) {
	char msg[BUFSIZ];
	sprintf( msg, "module '%s'", name() );
	SC_REPORT_WARNING( SC_ID_END_MODULE_NOT_CALLED_, msg );
	if( error_ ) {
	    SC_REPORT_WARNING( SC_ID_HIER_NAME_INCORRECT_, 0 );
	}
	error_ = true;
    }
    end_of_elaboration();
}

// called by start_simulation (does nothing by default)

void
sc_module::start_of_simulation()
{}

void
sc_module::start_simulation()
{
    start_of_simulation();
}

// called by simulation_done (does nothing by default)

void
sc_module::end_of_simulation()
{}

void
sc_module::simulation_done()
{
    end_of_simulation();
}

void
sc_module::set_stack_size( size_t size )
{
    sc_process_b*    proc_p;    // Current process.
    sc_thread_handle thread_h;  // Current process as thread.

    proc_p = (simcontext()->is_running()) ?
	sc_get_curr_process_handle() :
	sc_get_last_created_process_handle();

    thread_h = DCAST<sc_thread_handle>(proc_p);
    if ( thread_h ) 
    {
	thread_h->set_stack_size( size );
    }
    else
    {
	SC_REPORT_WARNING( SC_ID_SET_STACK_SIZE_, 0 );
    }
}


int
sc_module::append_port( sc_port_base* port_ )
{
    int index = m_port_vec->size();
    m_port_vec->push_back( port_ );
    return index;
}


// positional binding methods

sc_module&
sc_module::operator << ( sc_interface& interface_ )
{
    if( m_port_index == m_port_vec->size() ) {
	char msg[BUFSIZ];
	if( m_port_index == 0 ) {
	    sprintf( msg, "module `%s' has no ports", name() );
	} else {
	    sprintf( msg, "all ports of module `%s' are bound", name() );
	}
	SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_, msg );
    }
    int status = (*m_port_vec)[m_port_index]->pbind( interface_ );
    if( status != 0 ) {
	char msg[BUFSIZ];
	switch( status ) {
	case 1:
	    sprintf( msg, "port %d of module `%s' is already bound",
		     m_port_index, name() );
	    break;
	case 2:
	    sprintf( msg, "type mismatch on port %d of module `%s'",
		     m_port_index, name() );
	    break;
	default:
	    sprintf( msg, "unknown error" );
	    break;
	}
	SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_, msg );
    }
    ++ m_port_index;
    return *this;
}

sc_module&
sc_module::operator << ( sc_port_base& port_ )
{
    if( m_port_index == m_port_vec->size() ) {
	char msg[BUFSIZ];
	if( m_port_index == 0 ) {
	    sprintf( msg, "module `%s' has no ports", name() );
	} else {
	    sprintf( msg, "all ports of module `%s' are bound", name() );
	}
	SC_REPORT_ERROR( SC_ID_BIND_PORT_TO_PORT_, msg );
    }
    int status = (*m_port_vec)[m_port_index]->pbind( port_ );
    if( status != 0 ) {
	char msg[BUFSIZ];
	switch( status ) {
	case 1:
	    sprintf( msg, "port %d of module `%s' is already bound",
		     m_port_index, name() );
	    break;
	case 2:
	    sprintf( msg, "type mismatch on port %d of module `%s'",
		     m_port_index, name() );
	    break;
	default:
	    sprintf( msg, "unknown error" );
	    break;
	}
	SC_REPORT_ERROR( SC_ID_BIND_PORT_TO_PORT_, msg );
    }
    ++ m_port_index;
    return *this;
}


#define TRY_BIND( p )                                                         \
    if( (p).iface != 0 ) {                                                    \
        operator << ( *(p).iface );                                           \
    } else if( (p).port != 0 ) {                                              \
        operator << ( *(p).port );                                            \
    } else {                                                                  \
        return;                                                               \
    }


void
sc_module::operator () ( const sc_bind_proxy& p001,
			 const sc_bind_proxy& p002,
			 const sc_bind_proxy& p003,
			 const sc_bind_proxy& p004,
			 const sc_bind_proxy& p005,
			 const sc_bind_proxy& p006,
			 const sc_bind_proxy& p007,
			 const sc_bind_proxy& p008,
			 const sc_bind_proxy& p009,
			 const sc_bind_proxy& p010,
			 const sc_bind_proxy& p011,
			 const sc_bind_proxy& p012,
			 const sc_bind_proxy& p013,
			 const sc_bind_proxy& p014,
			 const sc_bind_proxy& p015,
			 const sc_bind_proxy& p016,
			 const sc_bind_proxy& p017,
			 const sc_bind_proxy& p018,
			 const sc_bind_proxy& p019,
			 const sc_bind_proxy& p020,
			 const sc_bind_proxy& p021,
			 const sc_bind_proxy& p022,
			 const sc_bind_proxy& p023,
			 const sc_bind_proxy& p024,
			 const sc_bind_proxy& p025,
			 const sc_bind_proxy& p026,
			 const sc_bind_proxy& p027,
			 const sc_bind_proxy& p028,
			 const sc_bind_proxy& p029,
			 const sc_bind_proxy& p030,
			 const sc_bind_proxy& p031,
			 const sc_bind_proxy& p032,
			 const sc_bind_proxy& p033,
			 const sc_bind_proxy& p034,
			 const sc_bind_proxy& p035,
			 const sc_bind_proxy& p036,
			 const sc_bind_proxy& p037,
			 const sc_bind_proxy& p038,
			 const sc_bind_proxy& p039,
			 const sc_bind_proxy& p040,
			 const sc_bind_proxy& p041,
			 const sc_bind_proxy& p042,
			 const sc_bind_proxy& p043,
			 const sc_bind_proxy& p044,
			 const sc_bind_proxy& p045,
			 const sc_bind_proxy& p046,
			 const sc_bind_proxy& p047,
			 const sc_bind_proxy& p048,
			 const sc_bind_proxy& p049,
			 const sc_bind_proxy& p050,
			 const sc_bind_proxy& p051,
			 const sc_bind_proxy& p052,
			 const sc_bind_proxy& p053,
			 const sc_bind_proxy& p054,
			 const sc_bind_proxy& p055,
			 const sc_bind_proxy& p056,
			 const sc_bind_proxy& p057,
			 const sc_bind_proxy& p058,
			 const sc_bind_proxy& p059,
			 const sc_bind_proxy& p060,
			 const sc_bind_proxy& p061,
			 const sc_bind_proxy& p062,
			 const sc_bind_proxy& p063,
			 const sc_bind_proxy& p064 )
{
    TRY_BIND( p001 );
    TRY_BIND( p002 );
    TRY_BIND( p003 );
    TRY_BIND( p004 );
    TRY_BIND( p005 );
    TRY_BIND( p006 );
    TRY_BIND( p007 );
    TRY_BIND( p008 );
    TRY_BIND( p009 );
    TRY_BIND( p010 );
    TRY_BIND( p011 );
    TRY_BIND( p012 );
    TRY_BIND( p013 );
    TRY_BIND( p014 );
    TRY_BIND( p015 );
    TRY_BIND( p016 );
    TRY_BIND( p017 );
    TRY_BIND( p018 );
    TRY_BIND( p019 );
    TRY_BIND( p020 );
    TRY_BIND( p021 );
    TRY_BIND( p022 );
    TRY_BIND( p023 );
    TRY_BIND( p024 );
    TRY_BIND( p025 );
    TRY_BIND( p026 );
    TRY_BIND( p027 );
    TRY_BIND( p028 );
    TRY_BIND( p029 );
    TRY_BIND( p030 );
    TRY_BIND( p031 );
    TRY_BIND( p032 );
    TRY_BIND( p033 );
    TRY_BIND( p034 );
    TRY_BIND( p035 );
    TRY_BIND( p036 );
    TRY_BIND( p037 );
    TRY_BIND( p038 );
    TRY_BIND( p039 );
    TRY_BIND( p040 );
    TRY_BIND( p041 );
    TRY_BIND( p042 );
    TRY_BIND( p043 );
    TRY_BIND( p044 );
    TRY_BIND( p045 );
    TRY_BIND( p046 );
    TRY_BIND( p047 );
    TRY_BIND( p048 );
    TRY_BIND( p049 );
    TRY_BIND( p050 );
    TRY_BIND( p051 );
    TRY_BIND( p052 );
    TRY_BIND( p053 );
    TRY_BIND( p054 );
    TRY_BIND( p055 );
    TRY_BIND( p056 );
    TRY_BIND( p057 );
    TRY_BIND( p058 );
    TRY_BIND( p059 );
    TRY_BIND( p060 );
    TRY_BIND( p061 );
    TRY_BIND( p062 );
    TRY_BIND( p063 );
    TRY_BIND( p064 );
}

} // namespace sc_core

// Taf!
