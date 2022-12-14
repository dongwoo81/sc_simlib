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

  sc_signal_resolved_ports.cpp -- The sc_signal_resolved port classes.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-08-20

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:
    
 *****************************************************************************/


#include <stdio.h>

#include "sysc/communication/sc_communication_ids.h"
#include "sysc/communication/sc_signal_resolved.h"
#include "sysc/communication/sc_signal_resolved_ports.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_in_resolved
//
//  The sc_signal_resolved input port class.
// ----------------------------------------------------------------------------

// called when elaboration is done

void
sc_in_resolved::end_of_elaboration()
{
    base_type::end_of_elaboration();
    // check if bound channel is a resolved signal
    if( DCAST<sc_signal_resolved*>( get_interface() ) == 0 ) {
	char msg[BUFSIZ];
	sprintf( msg, "%s (%s)", name(), kind() );
	SC_REPORT_ERROR( SC_ID_RESOLVED_PORT_NOT_BOUND_, msg );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : sc_inout_resolved
//
//  The sc_signal_resolved input/output port class.
// ----------------------------------------------------------------------------

// called when elaboration is done

void
sc_inout_resolved::end_of_elaboration()
{
    base_type::end_of_elaboration();
    // check if bound channel is a resolved signal
    if( DCAST<sc_signal_resolved*>( get_interface() ) == 0 ) {
	char msg[BUFSIZ];
	sprintf( msg, "%s (%s)", name(), kind() );
	SC_REPORT_ERROR( SC_ID_RESOLVED_PORT_NOT_BOUND_, msg );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : sc_out_resolved
//
//  The sc_signal_resolved output port class.
// ----------------------------------------------------------------------------

} // namespace sc_core

// Taf!
