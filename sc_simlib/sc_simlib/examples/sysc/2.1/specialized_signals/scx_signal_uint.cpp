/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2004 by all Contributors.
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

  sc_signal_uint.cpp -- The sc_signal<sc_dt::sc_uint<W> > implementations.

  Original Author: Andy Goodrich, Forte Design Systems, 2002-10-22

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

/* 
$Log: sc_signal_uint.cpp,v $
Revision 1.11  2005/04/11 19:05:36  acg
Change to sc_clock for Microsoft VCC 6.0. Changes for namespaces

Revision 1.10  2005/04/03 22:52:52  acg
Namespace changes.

Revision 1.9  2005/03/21 22:31:32  acg
Changes to sc_core namespace.

Revision 1.8  2004/09/27 21:01:59  acg
Andy Goodrich - Forte Design Systems, Inc.
   - This is specialized signal support that allows better use of signals
     and ports whose target value is a SystemC native type.

*/



#include "sysc/utils/sc_temporary.h"
#include "sysc/communication/sc_interface.h"
#include "sysc/communication/sc_port.h"
#include "sysc/communication/sc_prim_channel.h"
#include "sysc/communication/sc_signal_ports.h"
#include "sysc/communication/sc_signal.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_process_b.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_macros.h"
#include "sysc/tracing/sc_trace.h"
#include "sysc/datatypes/int/sc_nbdefs.h"
#include "sysc/datatypes/misc/sc_concatref.h"
#include "sysc/datatypes/int/sc_int.h"
#include "sysc/datatypes/int/sc_uint.h"
#include "sysc/datatypes/int/sc_signed.h"
#include "sysc/datatypes/int/sc_unsigned.h"
#include "sysc/datatypes/bit/sc_lv_base.h"
#include "./scx_signal_uint.h"
#include <typeinfo>

namespace sc_core {

//------------------------------------------------------------------------------
// POOL OF TEMPORARY INSTANCES OF sc_uint_sigref
//
// This allows use to pass const references for part and bit selections
// on sc_signal<sc_dt::sc_uint<W> > object instances.
//------------------------------------------------------------------------------
sc_vpool<sc_uint_sigref> sc_uint_sigref::m_pool(8);


//------------------------------------------------------------------------------
//"sc_uint_part_if::default methods"
//
// These versions just produce errors if they are not overloaded but used.
//------------------------------------------------------------------------------

sc_dt::sc_uint_base* sc_uint_part_if::part_read_target()
{
    SC_REPORT_ERROR( SC_ID_OPERATION_ON_NON_SPECIALIZED_SIGNAL_, "int" );
    return 0;
}
sc_dt::uint64 sc_uint_part_if::read_part( int left, int right ) const
{
    SC_REPORT_ERROR( SC_ID_OPERATION_ON_NON_SPECIALIZED_SIGNAL_, "int" );
    return 0;
}
sc_uint_sigref& sc_uint_part_if::select_part( int left, int right )
{
    SC_REPORT_ERROR( SC_ID_OPERATION_ON_NON_SPECIALIZED_SIGNAL_, "int" );
    return *(sc_uint_sigref*)0;
}
void sc_uint_part_if::write_part( sc_dt::uint64 v, int left, int right )
{
    SC_REPORT_ERROR( SC_ID_OPERATION_ON_NON_SPECIALIZED_SIGNAL_, "int" );
}

//------------------------------------------------------------------------------
//"sc_uint_sigref::concate_set"
//
// These methods assign this object instance's value from the supplied
// value starting at the supplied bit within that value.
//     src = value to use to set this object instance's value.
//     low_i = bit in src that is to be the low order bit of the value to set.
// #### OPTIMIZE
//------------------------------------------------------------------------------
void sc_uint_sigref::concat_set(sc_dt::int64 src, int low_i)
{
    *this = (low_i < 64) ? src >> low_i : src >> 63;
}


void sc_uint_sigref::concat_set(const sc_dt::sc_signed& src, int low_i)
{
    if ( low_i < src.length() )  
        *this = src >> low_i;
    else
        *this = (src < 0) ? (sc_dt::uint64)-1 : 0;
}


void sc_uint_sigref::concat_set(const sc_dt::sc_lv_base& src, int low_i)
{
    if ( low_i < src.length() )
        *this = (src >> low_i).to_uint64();
    else
        *this = 0;
}


void sc_uint_sigref::concat_set(const sc_dt::sc_unsigned& src, int low_i)
{
    if ( low_i < src.length() )
        *this = src >> low_i;
    else
        *this = 0;
}


void sc_uint_sigref::concat_set(sc_dt::uint64 src, int low_i)
{
    *this = (low_i < 64) ? src >> low_i : 0;
}

} // namespace sc_core
