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

  sc_uint_base.cpp -- contains interface definitions between sc_uint and
                 sc_signed, sc_unsigned, and definitions for sc_uint_subref.

  Original Author: Ali Dasdan, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "sysc/kernel/sc_macros.h"
#include "sysc/datatypes/int/sc_signed.h"
#include "sysc/datatypes/int/sc_unsigned.h"
#include "sysc/datatypes/int/sc_uint_base.h"
#include "sysc/datatypes/int/sc_int_ids.h"
#include "sysc/datatypes/bit/sc_bv_base.h"
#include "sysc/datatypes/bit/sc_lv_base.h"
#include "sysc/datatypes/misc/sc_concatref.h"
#include "sysc/datatypes/fx/sc_ufix.h"
#include "sysc/datatypes/fx/scfx_other_defs.h"


namespace sc_dt
{

// to avoid code bloat in sc_uint_concat<T1,T2>

void
sc_uint_concref_invalid_length( int length )
{
    char msg[BUFSIZ];
    sprintf( msg,
	     "sc_uint_concref<T1,T2> initialization: length = %d "
	     "violates 1 <= length <= %d",
	     length, SC_INTWIDTH );
    SC_REPORT_ERROR( sc_core::SC_ID_OUT_OF_BOUNDS_, msg );
}



// ----------------------------------------------------------------------------
//  CLASS : sc_uint_bitref
//
//  Proxy class for sc_uint bit selection (r-value and l-value).
// ----------------------------------------------------------------------------

sc_core::sc_vpool<sc_uint_bitref> sc_uint_bitref::m_pool(9);

// concatenation methods:

// #### OPTIMIZE
void sc_uint_bitref::concat_set(int64 src, int low_i)
{   
    sc_uint_base aa( 1 );
    *this = aa = (low_i < 64) ? src >> low_i : src >> 63;
}

void sc_uint_bitref::concat_set(const sc_signed& src, int low_i)
{
    sc_uint_base aa( 1 );     
    if ( low_i < src.length() )
        *this = aa = 1 & (src >> low_i);      
    else
        *this = aa = (src < 0) ? (int_type)-1 : 0;
}

void sc_uint_bitref::concat_set(const sc_unsigned& src, int low_i)
{
    sc_uint_base aa( 1 );
    if ( low_i < src.length() )
        *this = aa = 1 & (src >> low_i);
    else
        *this = aa = 0;
}

void sc_uint_bitref::concat_set(uint64 src, int low_i)
{
    sc_uint_base aa( 1 );
    *this = aa = (low_i < 64) ? src >> low_i : 0;
}


// other methods

void
sc_uint_bitref::scan( ::std::istream& is )
{
    bool b;
    is >> b;
    *this = b;
}


// ----------------------------------------------------------------------------
//  CLASS : sc_uint_subref_r
//
//  Proxy class for sc_uint part selection (l-value).
// ----------------------------------------------------------------------------

bool sc_uint_subref_r::concat_get_ctrl( unsigned long* dst_p, int low_i ) const
{
    int       dst_i;	   // Word in dst_p now processing.
    int       end_i;	   // Highest order word in dst_p to process.
    int       left_shift;  // Left shift for val.
    uint_type mask;	       // Mask for bits to extract or keep.

    dst_i = low_i / BITS_PER_DIGIT;
    left_shift = low_i % BITS_PER_DIGIT;
    end_i = (low_i + (m_left-m_right)) / BITS_PER_DIGIT; 

    mask = ~(-1 << left_shift);
    dst_p[dst_i] = (unsigned long)((dst_p[dst_i] & mask));

    dst_i++;
    for ( ; dst_i <= end_i; dst_i++ ) dst_p[dst_i] = 0;

    return false;
}

bool sc_uint_subref_r::concat_get_data( unsigned long* dst_p, int low_i ) const
{
    int       dst_i;	   // Word in dst_p now processing.
    int       end_i;	   // Highest order word in dst_p to process.
    int       high_i;	   // Index of high order bit in dst_p to set.
    int       left_shift;  // Left shift for val.
    uint_type mask;	   // Mask for bits to extract or keep.
    bool      result;	   // True if inserting non-zero value.
    uint_type val;	   // Selection value extracted from m_obj_p.

    dst_i = low_i / BITS_PER_DIGIT;
    left_shift = low_i % BITS_PER_DIGIT;
    high_i = low_i + (m_left-m_right); 
    end_i = high_i / BITS_PER_DIGIT;
    mask = ~mask_int[m_left][m_right];
    val = (m_obj_p->m_val & mask) >> m_right;
    result = val != 0;


    // PROCESS THE FIRST WORD:

    mask = ~(-1 << left_shift);
    dst_p[dst_i] = (unsigned long)(((dst_p[dst_i] & mask)) | 
	((val << left_shift) & DIGIT_MASK));

    switch ( end_i - dst_i )
    {
     // BITS ARE ACROSS TWO WORDS:

     case 1:
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i] = (unsigned long)val;
        break;

     // BITS ARE ACROSS THREE WORDS:

     case 2:
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i++] = (unsigned long)(val & DIGIT_MASK);
        val >>= BITS_PER_DIGIT;
        dst_p[dst_i] = (unsigned long)val;
        break;

     // BITS ARE ACROSS THREE WORDS:

     case 3:
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i++] = (unsigned long)(val & DIGIT_MASK);
        val >>= BITS_PER_DIGIT;
        dst_p[dst_i++] = (unsigned long)(val & DIGIT_MASK);
        val >>= BITS_PER_DIGIT;
        dst_p[dst_i] = (unsigned long)val;
        break;
    }
    return result;
}

// ----------------------------------------------------------------------------
//  CLASS : sc_uint_subref
//
//  Proxy class for sc_uint part selection (r-value and l-value).
// ----------------------------------------------------------------------------

sc_core::sc_vpool<sc_uint_subref> sc_uint_subref::m_pool(9);

// assignment operators

sc_uint_subref& 
sc_uint_subref::operator = ( uint_type v )
{
    uint_type val = m_obj_p->m_val;
    uint_type mask = mask_int[m_left][m_right];
    val &= mask;
    val |= (v << m_right) & ~mask;
    m_obj_p->m_val = val;
    m_obj_p->extend_sign();
    return *this;
}

sc_uint_subref&
sc_uint_subref::operator = ( const sc_signed& a )
{
    sc_uint_base aa( length() );
    return ( *this = aa = a );
}

sc_uint_subref&
sc_uint_subref::operator = ( const sc_unsigned& a )
{
    sc_uint_base aa( length() );
    return ( *this = aa = a );
}

sc_uint_subref&
sc_uint_subref::operator = ( const sc_bv_base& a )
{
    sc_uint_base aa( length() );
    return ( *this = aa = a );
}

sc_uint_subref&
sc_uint_subref::operator = ( const sc_lv_base& a )
{
    sc_uint_base aa( length() );
    return ( *this = aa = a );
}

// concatenation methods:

// #### OPTIMIZE
void sc_uint_subref::concat_set(int64 src, int low_i)
{
    sc_uint_base aa( length() );
    *this = aa = (low_i < 64) ? src >> low_i : src >> 63;
}

void sc_uint_subref::concat_set(const sc_signed& src, int low_i)   
{
    sc_uint_base aa( length() );   
    if ( low_i < src.length() )
        *this = aa = src >> low_i;
    else
        *this = aa = (src < 0) ? (int_type)-1 : 0;
}

void sc_uint_subref::concat_set(const sc_unsigned& src, int low_i)   
{
    sc_uint_base aa( length() );
    if ( low_i < src.length() )
        *this = aa = src >> low_i;
    else
        *this = aa = 0;
} 

void sc_uint_subref::concat_set(uint64 src, int low_i)   
{      
    sc_uint_base aa( length() );
    *this = aa = (low_i < 64) ? src >> low_i : 0;
}

// other methods

void
sc_uint_subref::scan( ::std::istream& is )
{
    std::string s;
    is >> s;
    *this = s.c_str();
}


// ----------------------------------------------------------------------------
//  CLASS : sc_uint_base
//
//  Base class for sc_uint.
// ----------------------------------------------------------------------------

// support methods

void
sc_uint_base::invalid_length() const
{
    char msg[BUFSIZ];
    sprintf( msg,
	     "sc_uint[_base] initialization: length = %d violates "
	     "1 <= length <= %d",
	     m_len, SC_INTWIDTH );
    SC_REPORT_ERROR( sc_core::SC_ID_OUT_OF_BOUNDS_, msg );
}

void
sc_uint_base::invalid_index( int i ) const
{
    char msg[BUFSIZ];
    sprintf( msg,
	     "sc_uint[_base] bit selection: index = %d violates "
	     "0 <= index <= %d",
	     i, m_len - 1 );
    SC_REPORT_ERROR( sc_core::SC_ID_OUT_OF_BOUNDS_, msg );
}

void
sc_uint_base::invalid_range( int l, int r ) const
{
    char msg[BUFSIZ];
    sprintf( msg,
	     "sc_uint[_base] part selection: left = %d, right = %d violates "
	     "0 <= right <= left <= %d",
	     l, r, m_len - 1 );
    SC_REPORT_ERROR( sc_core::SC_ID_OUT_OF_BOUNDS_, msg );
}


void
sc_uint_base::check_value() const
{
    uint_type limit = (~UINT_ZERO >> m_ulen);
    if( m_val > limit ) {
	char msg[BUFSIZ];
	sprintf( msg, "sc_uint[_base]: value does not fit into a length of %d",
		 m_len );
	SC_REPORT_WARNING( sc_core::SC_ID_OUT_OF_BOUNDS_, msg );
    }
}


// constructors

sc_uint_base::sc_uint_base( const sc_bv_base& v ) 
    : m_val(0), m_len( v.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
    *this = v;
}
sc_uint_base::sc_uint_base( const sc_lv_base& v )
    : m_val(0), m_len( v.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
    *this = v;
}
sc_uint_base::sc_uint_base( const sc_int_subref_r& v )
    : m_val(0), m_len( v.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
    *this = v.to_uint64();
}
sc_uint_base::sc_uint_base( const sc_signed_subref_r& v )
    : m_val(0), m_len( v.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
    *this = v.to_uint64();
}
sc_uint_base::sc_uint_base( const sc_unsigned_subref_r& v )
    : m_val(0), m_len( v.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
    *this = v.to_uint64();
}

sc_uint_base::sc_uint_base( const sc_signed& a )
    : m_val( 0 ), m_len( a.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
#if 0
    for( int i = m_len - 1; i >= 0; -- i ) {
	set( i, a.test( i ) );
    }
    extend_sign();
#else
    *this = a.to_uint64();
#endif
}

sc_uint_base::sc_uint_base( const sc_unsigned& a )
    : m_val( 0 ), m_len( a.length() ), m_ulen( SC_INTWIDTH - m_len )
{
    check_length();
#if 0
    for( int i = m_len - 1; i >= 0; -- i ) {
	set( i, a.test( i ) );
    }
    extend_sign();
#else
    *this = a.to_uint64();
#endif
}

// assignment operators

sc_uint_base&
sc_uint_base::operator = ( const sc_signed& a )
{
    int minlen = sc_min( m_len, a.length() );
    int i = 0;
    for( ; i < minlen; ++ i ) {
	set( i, a.test( i ) );
    }
    bool sgn = a.sign();
    for( ; i < m_len; ++ i ) {
	// sign extension
	set( i, sgn );
    }
    extend_sign();
    return *this;
}

sc_uint_base& 
sc_uint_base::operator = ( const sc_unsigned& a )
{
    int minlen = sc_min( m_len, a.length() );
    int i = 0;
    for( ; i < minlen; ++ i ) {
	set( i, a.test( i ) );
    }
    for( ; i < m_len; ++ i ) {
	// zero extension
	set( i, 0 );
    }
    extend_sign();
    return *this;
}


sc_uint_base&
sc_uint_base::operator = ( const sc_bv_base& a )
{
    int minlen = sc_min( m_len, a.length() );
    int i = 0;
    for( ; i < minlen; ++ i ) {
	set( i, a.get_bit( i ) );
    }
    for( ; i < m_len; ++ i ) {
	// zero extension
	set( i, 0 );
    }
    extend_sign();
    return *this;
}

sc_uint_base&
sc_uint_base::operator = ( const sc_lv_base& a )
{
    int minlen = sc_min( m_len, a.length() );
    int i = 0;
    for( ; i < minlen; ++ i ) {
	set( i, sc_logic( a.get_bit( i ) ).to_bool() );
    }
    for( ; i < m_len; ++ i ) {
	// zero extension
	set( i, 0 );
    }
    extend_sign();
    return *this;
}

sc_uint_base&
sc_uint_base::operator = ( const char* a )
{
    if( a == 0 ) {
	SC_REPORT_ERROR( sc_core::SC_ID_CONVERSION_FAILED_,
			 "character string is zero" );
    }
    if( *a == 0 ) {
	SC_REPORT_ERROR( sc_core::SC_ID_CONVERSION_FAILED_,
			 "character string is empty" );
    }
    try {
	int len = m_len;
	sc_ufix aa( a, len, len, SC_TRN, SC_WRAP, 0, SC_ON );
	return this->operator = ( aa );
    } catch( sc_core::sc_report ) {
	char msg[BUFSIZ];
	sprintf( msg, "character string '%s' is not valid", a );
	SC_REPORT_ERROR( sc_core::SC_ID_CONVERSION_FAILED_, msg );
	// never reached
	return *this;
    }
}


// explicit conversion to character string

const std::string
sc_uint_base::to_string( sc_numrep numrep ) const
{
    int len = m_len;
    sc_ufix aa( *this, len, len, SC_TRN, SC_WRAP, 0, SC_ON );
    return aa.to_string( numrep );
}

const std::string
sc_uint_base::to_string( sc_numrep numrep, bool w_prefix ) const
{
    int len = m_len;
    sc_ufix aa( *this, len, len, SC_TRN, SC_WRAP, 0, SC_ON );
    return aa.to_string( numrep, w_prefix );
}


// reduce methods

bool
sc_uint_base::and_reduce() const
{
    return ( m_val == (~UINT_ZERO >> m_ulen) );
}

bool
sc_uint_base::or_reduce() const
{
    return ( m_val != uint_type( 0 ) );
}

bool
sc_uint_base::xor_reduce() const
{
    uint_type mask = ~UINT_ZERO;
    uint_type val = m_val;
    int n = SC_INTWIDTH;
    do {
	n >>= 1;
	mask >>= n;
	val = ((val & (mask << n)) >> n) ^ (val & mask);
    } while( n != 1 );
    return ( val != uint_type( 0 ) );
}


bool sc_uint_base::concat_get_ctrl( unsigned long* dst_p, int low_i ) const
{    
    int       dst_i;       // Word in dst_p now processing.
    int       end_i;       // Highest order word in dst_p to process.
    int       left_shift;  // Left shift for val.
    uint_type mask;        // Mask for bits to extract or keep.

    dst_i = low_i / BITS_PER_DIGIT;
    left_shift = low_i % BITS_PER_DIGIT;
    end_i = (low_i + (m_len-1)) / BITS_PER_DIGIT;

    // PROCESS THE FIRST WORD:

    mask = ~((uint_type)-1 << left_shift);
    dst_p[dst_i] = (unsigned long)((dst_p[dst_i] & mask));

    dst_i++;
    for ( ; dst_i <= end_i; dst_i++ ) dst_p[dst_i] = 0;
    return false;
}

bool sc_uint_base::concat_get_data( unsigned long* dst_p, int low_i ) const
{    
    int       dst_i;       // Word in dst_p now processing.
    int       end_i;       // Highest order word in dst_p to process.
    int       high_i;      // Index of high order bit in dst_p to set.
    int       left_shift;  // Left shift for val.
    uint_type mask;        // Mask for bits to extract or keep.
    bool      result;	   // True if inserting non-zero value.
    uint_type val;         // Value for this object.

    dst_i = low_i / BITS_PER_DIGIT;
    left_shift = low_i % BITS_PER_DIGIT;
    high_i = low_i + (m_len-1);
    end_i = high_i / BITS_PER_DIGIT;
    val = m_val;
    result = val != 0;

    // PROCESS THE FIRST WORD:

    mask = ~((uint_type)-1 << left_shift);
    dst_p[dst_i] = (unsigned long)(((dst_p[dst_i] & mask)) | 
	((val << left_shift) & DIGIT_MASK));

    switch ( end_i - dst_i )
    {
     // BITS ARE ACROSS TWO WORDS:

     case 1:
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i] = (unsigned long)val;
        break;

     // BITS ARE ACROSS THREE WORDS:

     case 2:
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i] = (unsigned long)(val & DIGIT_MASK);
        dst_i++;
        val >>= (BITS_PER_DIGIT-left_shift);
        dst_p[dst_i] = (unsigned long)val;
        break;
    }
    return result;
}

// #### OPTIMIZE
void sc_uint_base::concat_set(int64 src, int low_i)
{
    *this = (low_i < 64) ? src >> low_i : src >> 63;
}

void sc_uint_base::concat_set(const sc_signed& src, int low_i)
{
    if ( low_i < src.length() )
        *this = src >> low_i;                             
    else
        *this = (src < 0) ? (int_type)-1 : 0; 
}

void sc_uint_base::concat_set(const sc_unsigned& src, int low_i)
{
    if ( low_i < src.length() )
        *this = src >> low_i;
    else
        *this = 0;
}

void sc_uint_base::concat_set(uint64 src, int low_i)
{
    *this = (low_i < 64) ? src >> low_i : 0;
}


// other methods

void
sc_uint_base::scan( ::std::istream& is )
{
    std::string s;
    is >> s;
    *this = s.c_str();
}

} // namespace sc_dt


// Taf!
