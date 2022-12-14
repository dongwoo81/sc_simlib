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

  sc_clock.cpp -- The clock channel.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Bishnupriya Bhattacharya, Cadence Design Systems,
                               Andy Goodrich, Forte Design Systems,
                               3 October, 2003
  Description of Modification: sc_clock inherits from sc_signal<bool> only
                               instead of sc_signal_in_if<bool> and sc_module.
                               The 2 methods posedge_action() and
                               negedge_action() are created using sc_spawn().
                               boost::bind() is not required, instead a local
                               bind function can be used since the signatures
                               of the spawned functions are statically known.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "sysc/communication/sc_clock.h"
#include "sysc/communication/sc_communication_ids.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_process_base.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_clock
//
//  The clock channel.
// ----------------------------------------------------------------------------

// constructors

sc_clock::sc_clock()
: sc_signal<bool>( sc_gen_unique_name( "clock" ) )
{
    init( sc_time( 1.0, true ),
	  0.5,
	  SC_ZERO_TIME,
	  true );

    m_next_posedge_event.notify_delayed( m_start_time );
}

sc_clock::sc_clock( const char* name_ )
: sc_signal<bool>( name_ )
{
    init( sc_time( 1.0, true ),
	  0.5,
	  SC_ZERO_TIME,
	  true );

    m_next_posedge_event.notify_delayed( m_start_time );
}

sc_clock::sc_clock( const char* name_,
		    const sc_time& period_,
		    double         duty_cycle_,
		    const sc_time& start_time_,
		    bool           posedge_first_ )
: sc_signal<bool>( name_ )
{
    init( period_,
	  duty_cycle_,
	  start_time_,
	  posedge_first_ );

    if( posedge_first_ ) {
	// posedge first
	m_next_posedge_event.notify_delayed( m_start_time );
    } else {
	// negedge first
	m_next_negedge_event.notify_delayed( m_start_time );
    }
}

sc_clock::sc_clock( const char* name_,
		    double         period_v_,
		    sc_time_unit   period_tu_,
		    double         duty_cycle_ )
: sc_signal<bool>( name_ )
{
    init( sc_time( period_v_, period_tu_, simcontext() ),
	  duty_cycle_,
	  SC_ZERO_TIME,
	  true );

    // posedge first
    m_next_posedge_event.notify_delayed( m_start_time );
}

sc_clock::sc_clock( const char* name_,
		    double         period_v_,
		    sc_time_unit   period_tu_,
		    double         duty_cycle_,
		    double         start_time_v_,
		    sc_time_unit   start_time_tu_,
		    bool           posedge_first_ )
: sc_signal<bool>( name_ )
{
    init( sc_time( period_v_, period_tu_, simcontext() ),
	  duty_cycle_,
	  sc_time( start_time_v_, start_time_tu_, simcontext() ),
	  posedge_first_ );

    if( posedge_first_ ) {
	// posedge first
	m_next_posedge_event.notify_delayed( m_start_time );
    } else {
	// negedge first
	m_next_negedge_event.notify_delayed( m_start_time );
    }
}

// for backward compatibility with 1.0
sc_clock::sc_clock( const char* name_,
		    double         period_,      // in default time units
		    double         duty_cycle_,
		    double         start_time_,  // in default time units
		    bool           posedge_first_ )
: sc_signal<bool>( name_ )
{
    init( sc_time( period_, true ),
	  duty_cycle_,
	  sc_time( start_time_, true ),
	  posedge_first_ );

    if( posedge_first_ ) {
	// posedge first
	m_next_posedge_event.notify_delayed( m_start_time );
    } else {
	// negedge first
	m_next_negedge_event.notify_delayed( m_start_time );
    }
}


//------------------------------------------------------------------------------
//"sc_clock::before_end_of_elaboration"
//
// This callback is used to spawn the edge processes for this object instance.
// The processes are created here rather than the constructor for the object
// so that the processes are registered with the global simcontext rather
// than the scope of the clock's parent.
//------------------------------------------------------------------------------
#if ( defined(_MSC_VER) && _MSC_VER < 1300 ) //VC++6.0 doesn't support sc_spawn with functor
#   define sc_clock_posedge_callback(ptr) sc_clock_posedge_callback

#   define sc_clock_negedge_callback(ptr) sc_clock_negedge_callback

#   define sc_spawn(a,b,c) { \
        sc_process_handle result(new sc_spawn_object<a>(a(this),b,c)); \
    }
#endif // ( defined(_MSC_VER) && _MSC_VER < 1300 )

void sc_clock::before_end_of_elaboration()
{
    std::string gen_base;
    sc_spawn_options posedge_options;	// Options for posedge process.
    sc_spawn_options negedge_options;	// Options for negedge process.

    posedge_options.spawn_method();
    posedge_options.dont_initialize();
    posedge_options.set_sensitivity(&m_next_posedge_event);
    gen_base = basename();
    gen_base += "_posedge_action";
    sc_spawn(sc_clock_posedge_callback(this),
	sc_gen_unique_name( gen_base.c_str() ), &posedge_options);

    negedge_options.spawn_method();
    negedge_options.dont_initialize();
    negedge_options.set_sensitivity(&m_next_negedge_event);
    gen_base = basename();
    gen_base += "_negedge_action";
    sc_spawn( sc_clock_negedge_callback(this),
    	sc_gen_unique_name( gen_base.c_str() ), &negedge_options );
}

//clear VC++6.0 macros
#undef sc_clock_posedge_callback
#undef sc_clock_negedge_callback
#undef sc_spawn

// destructor (does nothing)

sc_clock::~sc_clock()
{}

void
sc_clock::write( const bool& value)
{
    SC_REPORT_ERROR(SC_ID_ATTEMPT_TO_WRITE_TO_CLOCK_, "");
}

// interface methods

// get the current time

const sc_time&
sc_clock::time_stamp()
{
    return sc_time_stamp();
}


// error reporting

void
sc_clock::report_error( const char* id, const char* add_msg ) const
{
    char msg[BUFSIZ];
    if( add_msg != 0 ) {
	sprintf( msg, "%s: clock '%s'", add_msg, name() );
    } else {
	sprintf( msg, "clock '%s'", name() );
    }
    SC_REPORT_ERROR( id, msg );
}


void
sc_clock::init( const sc_time& period_,
		double         duty_cycle_,
		const sc_time& start_time_,
		bool           posedge_first_ )
{
    if( period_ == SC_ZERO_TIME ) {
	report_error( SC_ID_CLOCK_PERIOD_ZERO_,
		      "increase the period" );
    }
    m_period = period_;
    m_posedge_first = posedge_first_;

    if( duty_cycle_ <= 0.0 || duty_cycle_ >= 1.0 ) {
	m_duty_cycle = 0.5;
    } else {
	m_duty_cycle = duty_cycle_;
    }

    m_negedge_time = m_period * m_duty_cycle;
    m_posedge_time = m_period - m_negedge_time;

    if( m_negedge_time == SC_ZERO_TIME ) {
	report_error( SC_ID_CLOCK_HIGH_TIME_ZERO_,
		      "increase the period or increase the duty cycle" );
    }
    if( m_posedge_time == SC_ZERO_TIME ) {
	report_error( SC_ID_CLOCK_LOW_TIME_ZERO_,
		      "increase the period or decrease the duty cycle" );
    }

    if( posedge_first_ ) {
	this->m_cur_val = false;
	this->m_new_val = false;
    } else {
	this->m_cur_val = true;
	this->m_new_val = true;
    }

    m_start_time = start_time_;

}

} // namespace sc_core


// Taf!
