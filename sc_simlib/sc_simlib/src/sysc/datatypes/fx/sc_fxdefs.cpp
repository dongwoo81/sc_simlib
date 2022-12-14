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

  sc_fxdefs.cpp - 

  Original Author: Martin Janssen, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "sysc/datatypes/fx/sc_fxdefs.h"


namespace sc_dt
{

// ----------------------------------------------------------------------------
//  ENUM : sc_enc
//
//  Enumeration of sign encodings.
// ----------------------------------------------------------------------------

const std::string
to_string( sc_enc enc )
{
    switch( enc )
    {
        case SC_TC_:
            return std::string( "SC_TC_" );
        case SC_US_:
            return std::string( "SC_US_" );
	default:
	    return std::string( "unknown" );
    }
}


// ----------------------------------------------------------------------------
//  ENUM : sc_q_mode
//
//  Enumeration of quantization modes.
// ----------------------------------------------------------------------------

const std::string
to_string( sc_q_mode q_mode )
{
    switch( q_mode )
    {
        case SC_RND:
            return std::string( "SC_RND" );
        case SC_RND_ZERO:
            return std::string( "SC_RND_ZERO" );
        case SC_RND_MIN_INF:
            return std::string( "SC_RND_MIN_INF" );
        case SC_RND_INF:
            return std::string( "SC_RND_INF" );
        case SC_RND_CONV:
            return std::string( "SC_RND_CONV" );
        case SC_TRN:
            return std::string( "SC_TRN" );
        case SC_TRN_ZERO:
            return std::string( "SC_TRN_ZERO" );
	default:
	    return std::string( "unknown" );
    }
}


// ----------------------------------------------------------------------------
//  ENUM : sc_o_mode
//
//  Enumeration of overflow modes.
// ----------------------------------------------------------------------------

const std::string
to_string( sc_o_mode o_mode )
{
    switch( o_mode )
    {
        case SC_SAT:
            return std::string( "SC_SAT" );
        case SC_SAT_ZERO:
            return std::string( "SC_SAT_ZERO" );
        case SC_SAT_SYM:
            return std::string( "SC_SAT_SYM" );
        case SC_WRAP:
            return std::string( "SC_WRAP" );
        case SC_WRAP_SM:
            return std::string( "SC_WRAP_SM" );
	default:
	    return std::string( "unknown" );
    }
}


// ----------------------------------------------------------------------------
//  ENUM : sc_switch
//
//  Enumeration of switch states.
// ----------------------------------------------------------------------------

const std::string
to_string( sc_switch sw )
{
    switch( sw ) {
        case SC_OFF:
            return std::string( "SC_OFF" );
        case SC_ON:
            return std::string( "SC_ON" );
	default:
	    return std::string( "unknown" );
    }
}


// ----------------------------------------------------------------------------
//  ENUM : sc_fmt
//
//  Enumeration of formats for character string conversion.
// ----------------------------------------------------------------------------

const std::string
to_string( sc_fmt fmt )
{
    switch( fmt ) {
        case SC_F:
            return std::string( "SC_F" );
        case SC_E:
            return std::string( "SC_E" );
	default:
	    return std::string( "unknown" );
    }
}

} // namespace sc_dt


// Taf!
