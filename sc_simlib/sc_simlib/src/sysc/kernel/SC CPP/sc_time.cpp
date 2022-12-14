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

  sc_time.cpp --

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include <math.h>
#include <stdio.h>

#include "sysc/kernel/sc_kernel_ids.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_time.h"

namespace sc_core {

static
double time_values[] = {
    1,       // fs
    1e3,     // ps
    1e6,     // ns
    1e9,     // us
    1e12,    // ms
    1e15     // s
};

static
const char* time_units[] = {
    "fs",
    "ps",
    "ns",
    "us",
    "ms",
    "s"
};


// ----------------------------------------------------------------------------
//  CLASS : sc_time
//
//  The time class.
// ----------------------------------------------------------------------------

// constructors

sc_time::sc_time( double v, sc_time_unit tu )
: m_value( 0 )
{
    if( v != 0 ) {
	sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
	double scale_fac = time_values[tu] / time_params->time_resolution;
	// linux bug workaround; don't change next two lines
	volatile double tmp = v * scale_fac + 0.5;
	m_value = SCAST<sc_dt::int64>( tmp );
	time_params->time_resolution_fixed = true;
    }
}

sc_time::sc_time( double v, sc_time_unit tu, sc_simcontext* simc )
: m_value( 0 )
{
    if( v != 0 ) {
	sc_time_params* time_params = simc->m_time_params;
	double scale_fac = time_values[tu] / time_params->time_resolution;
	// linux bug workaround; don't change next two lines
	volatile double tmp = v * scale_fac + 0.5;
	m_value = SCAST<sc_dt::int64>( tmp );
	time_params->time_resolution_fixed = true;
    }
}

sc_time::sc_time( double v, bool scale )
: m_value( 0 )
{
    if( v != 0 ) {
	sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
	if( scale ) {
	    double scale_fac = sc_dt::uint64_to_double(
		time_params->default_time_unit );
	    // linux bug workaround; don't change next two lines
	    volatile double tmp = v * scale_fac + 0.5;
	    m_value = SCAST<sc_dt::int64>( tmp );
	} else {
	    // linux bug workaround; don't change next two lines
	    volatile double tmp = v + 0.5;
	    m_value = SCAST<sc_dt::int64>( tmp );
	}
	time_params->time_resolution_fixed = true;
    }
}

sc_time::sc_time( sc_dt::uint64 v, bool scale )
: m_value( 0 )
{
    if( v != 0 ) {
	sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
	if( scale ) {
	    double scale_fac = sc_dt::uint64_to_double(
		time_params->default_time_unit );
	    // linux bug workaround; don't change next two lines
	    volatile double tmp = sc_dt::uint64_to_double( v ) *
		                  scale_fac + 0.5;
	    m_value = SCAST<sc_dt::int64>( tmp );
	} else {
	    m_value = v;
	}
	time_params->time_resolution_fixed = true;
    }
}


// conversion functions

double
sc_time::to_default_time_units() const
{
    sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
    return ( sc_dt::uint64_to_double( m_value ) /
	     sc_dt::uint64_to_double( time_params->default_time_unit ) );
}

double
sc_time::to_seconds() const
{
    sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
    return ( sc_dt::uint64_to_double( m_value ) *
	     time_params->time_resolution * 1e-15 );
}

const std::string
sc_time::to_string() const
{
    sc_dt::uint64 val = m_value;
    if( val == 0 ) {
	return std::string( "0 s" );
    }
    sc_time_params* time_params = sc_get_curr_simcontext()->m_time_params;
    sc_dt::uint64 tr = SCAST<sc_dt::int64>( time_params->time_resolution );
    int n = 0;
    while( ( tr % 10 ) == 0 ) {
	tr /= 10;
	n ++;
    }
    assert( tr == 1 );
    while( ( val % 10 ) == 0 ) {
	val /= 10;
	n ++;
    }
    char buf[BUFSIZ];
#if !defined( _MSC_VER )
    sprintf( buf, "%llu", val );
#else
    sprintf( buf, "%I64u", val );
#endif
    std::string result( buf );
    if( n >= 15 ) {
	for( int i = n - 15; i > 0; -- i ) {
	    result += "0";
	}
	result += " s";
    } else {
	for( int i = n % 3; i > 0; -- i ) {
	    result += "0";
	}
	result += " ";
	result += time_units[n / 3];
    }
    return result;
}


// print function

void
sc_time::print( ::std::ostream& os ) const
{
    os << to_string();
}


// ----------------------------------------------------------------------------
//  STRUCT : sc_time_params
//
//  Struct that holds the time resolution and default time unit.
// ----------------------------------------------------------------------------

sc_time_params::sc_time_params()
: time_resolution( 1000 ),		// default 1 ps
  time_resolution_specified( false ),
  time_resolution_fixed( false ),
  default_time_unit( 1000 ),		// default 1 ns
  default_time_unit_specified( false )
{}

sc_time_params::~sc_time_params()
{}


// ----------------------------------------------------------------------------

// functions for accessing the time resolution and default time unit

void
sc_set_time_resolution( double v, sc_time_unit tu )
{
    // first perform the necessary checks

    // must be positive
    if( v < 0.0 ) {
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_, "value not positive" );
    }

    // must be a power of ten
    double dummy;
#if defined( __HP_aCC ) || defined(__ppc__)
    // aCC seems to have a bug in modf()
    if( modf( log10( v < 1.0 ? 1.0/v : v ), &dummy ) != 0.0 ) {
#else
    if( modf( log10( v ), &dummy ) != 0.0 ) {
#endif
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_,
			 "value not a power of ten" );
    }

    sc_simcontext* simc = sc_get_curr_simcontext();

    // can only be specified during elaboration
    if( simc->is_running() ) {
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_, "simulation running" );
    }

    sc_time_params* time_params = simc->m_time_params;

    // can be specified only once
    if( time_params->time_resolution_specified ) {
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_, "already specified" );
    }

    // can only be specified before any sc_time is constructed
    if( time_params->time_resolution_fixed ) {
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_,
			 "sc_time object(s) constructed" );
    }

    // must be larger than or equal to 1 fs
    volatile double resolution = v * time_values[tu];
    if( resolution < 1.0 ) {
	SC_REPORT_ERROR( SC_ID_SET_TIME_RESOLUTION_,
			 "value smaller than 1 fs" );
    }

    // recalculate the default time unit
    volatile double time_unit = sc_dt::uint64_to_double(
	time_params->default_time_unit ) *
	( time_params->time_resolution / resolution );
    if( time_unit < 1.0 ) {
	SC_REPORT_WARNING( SC_ID_DEFAULT_TIME_UNIT_CHANGED_, 0 );
	time_params->default_time_unit = 1;
    } else {
	time_params->default_time_unit = SCAST<sc_dt::int64>( time_unit );
    }

    time_params->time_resolution = resolution;
    time_params->time_resolution_specified = true;
}

sc_time 
sc_get_time_resolution()
{
    return sc_time( sc_dt::UINT64_ONE, false );
}


void
sc_set_default_time_unit( double v, sc_time_unit tu )
{
    // first perform the necessary checks

    // must be positive
    if( v < 0.0 ) {
	SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_, "value not positive" );
    }

    // must be a power of ten
    double dummy;
    if( modf( log10( v ), &dummy ) != 0.0 ) {
	SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_,
			 "value not a power of ten" );
    }

    sc_simcontext* simc = sc_get_curr_simcontext();

    // can only be specified during elaboration
    if( simc->is_running() ) {
	SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_, "simulation running" );
    }

    sc_time_params* time_params = simc->m_time_params;

    // can only be specified before any sc_time is constructed
    if( time_params->time_resolution_fixed ) {
        SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_,
                         "sc_time object(s) constructed" );
    }

    // can be specified only once
    if( time_params->default_time_unit_specified ) {
	SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_, "already specified" );
    }

    // must be larger than or equal to the time resolution
    volatile double time_unit = ( v * time_values[tu] ) /
	                        time_params->time_resolution;
    if( time_unit < 1.0 ) {
	SC_REPORT_ERROR( SC_ID_SET_DEFAULT_TIME_UNIT_,
			 "value smaller than time resolution" );
    }

    time_params->default_time_unit = SCAST<sc_dt::int64>( time_unit );
    time_params->default_time_unit_specified = true;
}

sc_time
sc_get_default_time_unit()
{
    return sc_time( sc_get_curr_simcontext()->m_time_params->default_time_unit,
		    false );
}


// ----------------------------------------------------------------------------

const sc_time SC_ZERO_TIME;


} // namespace sc_core
// Taf!
