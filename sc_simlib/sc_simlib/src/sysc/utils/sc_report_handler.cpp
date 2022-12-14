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

  sc_report_handler.cpp - 

  Original Author: Alex Riesen, Synopsys, Inc.
  see also sc_report.cpp

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "sysc/utils/sc_iostream.h"
#include "sysc/kernel/sc_process_int.h"
#include "sysc/kernel/sc_simcontext_int.h"
#include "sysc/utils/sc_stop_here.h"
#include "sysc/utils/sc_report_handler.h"
#include "sysc/utils/sc_report.h"

namespace std {}

namespace sc_core {

// not documented, but available
const std::string sc_report_compose_message(const sc_report& rep)
{
    static const char * severity_names[] = {
	"Info", "Warning", "Error", "Fatal"
    };
    std::string str;

    str += severity_names[rep.get_severity()];
    str += ": ";

    if ( rep.get_id() >= 0 ) // backward compatibility with 2.0+
    {
	char idstr[64];
	sprintf(idstr, "(%c%d) ",
		"IWEF"[rep.get_severity()], rep.get_id());
	str += idstr;
    }
    str += rep.get_msg_type();

    if( *rep.get_msg() )
    {
	str += ": ";
	str += rep.get_msg();
    }
    if( rep.get_severity() > SC_INFO )
    {
        char line_number_str[16];
	str += "\nIn file: ";
	str += rep.get_file_name();
	str += ":";
	sprintf(line_number_str, "%d", rep.get_line_number());
	str += line_number_str;
	sc_simcontext* simc = sc_get_curr_simcontext();

	if( simc && simc->is_running() )
	{
	    const char* proc_name = rep.get_process_name();

	    if( proc_name )
	    {
		str += "\nIn process: ";
		str += proc_name;
		str += " @ ";
		str += rep.get_time().to_string();
	    }
	}
    }

    return str;
}
bool sc_report_close_default_log();

static ::std::ofstream* log_stream = 0;
static
struct auto_close_log
{
    ~auto_close_log()
    {
	sc_report_close_default_log();
    }
} auto_close;

const char* sc_report::get_process_name() const
{
	return process ? process->name() : 0;
}


//
// The official handler of the exception reporting
//

void sc_report_handler::default_handler(const sc_report& rep,
					const sc_actions& actions)
{
    if ( actions & SC_DISPLAY )
	::std::cout << ::std::endl << sc_report_compose_message(rep) << 
		::std::endl;

    if ( (actions & SC_LOG) && get_log_file_name() )
    {
	if ( !log_stream )
	    log_stream = new ::std::ofstream(get_log_file_name()); // ios::trunc

	*log_stream << rep.get_time() << ": "
	    << sc_report_compose_message(rep) << ::std::endl;
    }
    if ( actions & SC_STOP )
    {
	sc_stop_here((int)rep.get_msg_type(), rep.get_severity());
	sc_stop();
    }
    if ( actions & SC_INTERRUPT )
	sc_interrupt_here((int)rep.get_msg_type(), rep.get_severity());

    if ( actions & SC_ABORT )
	abort();

    if ( actions & SC_THROW )
	throw rep; 
}

// not documented, but available
bool sc_report_close_default_log()
{
    delete log_stream;
    sc_report_handler::set_log_file_name(NULL);

    if ( !log_stream )
	return false;

    log_stream = 0;
    return true;
}

int sc_report_handler::get_count(sc_severity severity_) 
{ 
   return sev_call_count[severity_]; 
} 

int sc_report_handler::get_count(const char* msg_type_) 
{ 
    sc_msg_def * md = mdlookup(msg_type_); 

    if ( !md ) 
        md = add_msg_type(msg_type_); 

    return md->call_count; 
} 

int sc_report_handler::get_count(const char* msg_type_, sc_severity severity_) 
{ 
    sc_msg_def * md = mdlookup(msg_type_); 

    if ( !md ) 
        md = add_msg_type(msg_type_); 

    return md->sev_call_count[severity_]; 
} 


//
// CLASS: sc_report_handler
// implementation
//

sc_msg_def * sc_report_handler::mdlookup(const char * msg_type_)
{
    for ( msg_def_items * item = messages; item; item = item->next )
    {
	for ( int i = 0; i < item->count; ++i )
	    if ( !strcmp(msg_type_, item->md[i].msg_type) )
		return item->md + i;
    }
    return 0;
}

// The calculation of actions to be executed
sc_actions sc_report_handler::execute(sc_msg_def* md, sc_severity severity_)
{
    sc_actions actions = md->sev_actions[severity_]; // high prio

    if ( SC_UNSPECIFIED == actions ) // middle prio
	actions = md->actions;

    if ( SC_UNSPECIFIED == actions ) // the lowest prio
	actions = sev_actions[severity_];

    actions &= ~suppress_mask; // higher than the high prio
    actions |= force_mask; // higher than above, and the limit is the highest

    unsigned * limit = 0;
    unsigned * call_count = 0;

    // just increment counters and check for overflow
    if ( md->sev_call_count[severity_] < UINT_MAX )
	md->sev_call_count[severity_]++;
    if ( md->call_count < UINT_MAX )
	md->call_count++;
    if ( sev_call_count[severity_] < UINT_MAX )
	sev_call_count[severity_]++;

    if ( md->limit_mask & (1 << (severity_ + 1)) )
    {
	limit = md->sev_limit + severity_;
	call_count = md->sev_call_count + severity_;
    }
    if ( !limit && (md->limit_mask & 1) )
    {
	limit = &md->limit;
	call_count = &md->call_count;
    }
    if ( !limit )
    {
	limit = sev_limit + severity_;
	call_count = sev_call_count + severity_;
    }
    if ( *limit == 0 )
	;// stop limit disabled
    else if ( *limit == UINT_MAX )
    {
	if ( severity_ >= SC_FATAL ) // sc_stop disabled, but FATAL+ sc_stops
	    actions |= SC_STOP; // force sc_stop()
    }
    else
    {
	if ( *call_count >= *limit )
	    actions |= SC_STOP; // force sc_stop()
    }
    return actions;
}

void sc_report_handler::report(sc_severity severity_,
			       const char * msg_type_,
			       const char * msg_,
			       const char * file_,
			       int line_)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( !md )
	md = add_msg_type(msg_type_);

    sc_actions actions = execute(md, severity_);
    sc_report rep(severity_, md, msg_, file_, line_);

    if ( actions & SC_CACHE_REPORT )
	cache_report(rep);

    handler(rep, actions);
}

void sc_report_handler::initialize()
{
#if 0 // actually, i do not know whether we have to reset these.
    suppress();
    force();
    set_actions(SC_INFO,    SC_DEFAULT_INFO_ACTIONS);
    set_actions(SC_WARNING, SC_DEFAULT_WARNING_ACTIONS);
    set_actions(SC_ERROR,   SC_DEFAULT_ERROR_ACTIONS);
    set_actions(SC_FATAL,   SC_DEFAULT_FATAL_ACTIONS);
#endif

    sev_call_count[SC_INFO]    = 0;
    sev_call_count[SC_WARNING] = 0;
    sev_call_count[SC_ERROR]   = 0;
    sev_call_count[SC_FATAL]   = 0;

    msg_def_items * items = messages;

    while ( items != &msg_terminator )
    {
	for ( int i = 0; i < items->count; ++i )
	{
	    items->md[i].call_count = 0;
	    items->md[i].sev_call_count[SC_INFO]    = 0;
	    items->md[i].sev_call_count[SC_WARNING] = 0;
	    items->md[i].sev_call_count[SC_ERROR]   = 0;
	    items->md[i].sev_call_count[SC_FATAL]   = 0;
	}
	items = items->next;
    }
}

// free the sc_msg_def's allocated by add_msg_type
// (or implicit msg_type registration: set_actions, abort_after)
// clear last_global_report.
void sc_report_handler::release()
{
    if ( last_global_report )
	delete last_global_report;
    last_global_report = 0;
    sc_report_close_default_log();

    msg_def_items * items = messages, * newitems = &msg_terminator;
    messages = &msg_terminator;

    while ( items != &msg_terminator )
    {
	for ( int i = 0; i < items->count; ++i )
	    if ( items->md[i].msg_type == items->md[i].msg_type_data )
		free(items->md[i].msg_type_data);

	msg_def_items * prev = items;
	items = items->next;

	if ( prev->allocated )
	{
	    delete [] prev->md;
	    delete prev;
	}
	else
	{
	    prev->next = newitems;
	    newitems = prev;
	}
    }
    messages = newitems;
}

sc_msg_def * sc_report_handler::add_msg_type(const char * msg_type_)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( md )
	return md;

    msg_def_items * items = new msg_def_items;

    if ( !items )
	return 0;

    items->count = 1;
    items->md = new sc_msg_def[items->count];

    if ( !items->md )
    {
	delete items;
	return 0;
    }
    memset(items->md, 0, sizeof(sc_msg_def) * items->count);
    items->md->msg_type_data = strdup(msg_type_);
    items->md->id = -1; // backward compatibility with 2.0+

    if ( !items->md->msg_type_data )
    {
	delete items->md;
	delete items;
	return 0;
    }
    items->md->msg_type = items->md->msg_type_data;
    add_static_msg_types(items);
    items->allocated = true;

    return items->md;
}

void sc_report_handler::add_static_msg_types(msg_def_items * items)
{
    items->allocated = false;
    items->next = messages;
    messages = items;
}

sc_actions sc_report_handler::set_actions(sc_severity severity_,
					  sc_actions actions_)
{
    sc_actions old = sev_actions[severity_];
    sev_actions[severity_] = actions_;
    return old;
}

sc_actions sc_report_handler::set_actions(const char * msg_type_,
					  sc_actions actions_)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( !md )
	md = add_msg_type(msg_type_);

    sc_actions old = md->actions;
    md->actions = actions_;

    return old;
}

sc_actions sc_report_handler::set_actions(const char * msg_type_,
					  sc_severity severity_,
					  sc_actions actions_)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( !md )
	md = add_msg_type(msg_type_);

    sc_actions old = md->sev_actions[severity_];
    md->sev_actions[severity_] = actions_;

    return old;
}

int sc_report_handler::stop_after(sc_severity severity_, int limit)
{
    int old = sev_limit[severity_];

    sev_limit[severity_] = limit < 0 ? UINT_MAX: limit;

    return old;
}

int sc_report_handler::stop_after(const char * msg_type_, int limit)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( !md )
	md = add_msg_type(msg_type_);

    int old = md->limit_mask & 1 ? md->limit: UINT_MAX;

    if ( limit < 0 )
	md->limit_mask &= ~1;
    else
    {
	md->limit_mask |= 1;
	md->limit = limit;
    }
    return old;
}

int sc_report_handler::stop_after(const char * msg_type_,
				  sc_severity severity_,
				  int limit)
{
    sc_msg_def * md = mdlookup(msg_type_);

    if ( !md )
	md = add_msg_type(msg_type_);

    int mask = 1 << (severity_ + 1);
    int old = md->limit_mask & mask ?  md->sev_limit[severity_]: UINT_MAX;

    if ( limit < 0 )
	md->limit_mask &= ~mask;
    else
    {
	md->limit_mask |= mask;
	md->sev_limit[severity_] = limit;
    }
    return old;
}

sc_actions sc_report_handler::suppress(sc_actions mask)
{
    sc_actions old = suppress_mask;
    suppress_mask = mask;
    return old;
}

sc_actions sc_report_handler::suppress()
{
    return suppress(0);
}

sc_actions sc_report_handler::force(sc_actions mask)
{
    sc_actions old = force_mask;
    force_mask = mask;
    return old;
}

sc_actions sc_report_handler::force()
{
    return force(0);
}

void sc_report_handler::set_handler(sc_report_handler_proc handler_)
{
    handler = handler_ ? handler_: &sc_report_handler::default_handler;
}

sc_report* sc_report_handler::get_cached_report()
{
    sc_process_b * proc = sc_get_curr_process_handle();

    if ( proc )
	return proc->get_last_report();

    return last_global_report;
}

void sc_report_handler::clear_cached_report()
{
    sc_process_b * proc = sc_get_curr_process_handle();

    if ( proc )
	proc->set_last_report(0);
    else
    {
	if ( last_global_report )
	    delete last_global_report;
	last_global_report = 0;
    }
}

sc_actions sc_report_handler::get_new_action_id()
{
    for ( sc_actions p = 1; p; p <<= 1 )
    {
	if ( !(p & available_actions) ) // free
	{
	    available_actions |= p;
	    return p;
	}
    }
    return SC_UNSPECIFIED;
}

bool sc_report_handler::set_log_file_name(const char* name_)
{
    if ( !name_ )
    {
	free(log_file_name);
	log_file_name = 0;
	return false;
    }
    if ( log_file_name )
	return false;

    log_file_name = strdup(name_);
    return true;
}

const char * sc_report_handler::get_log_file_name()
{
    return log_file_name;
}

void sc_report_handler::cache_report(const sc_report& rep)
{
    sc_process_b * proc = sc_get_curr_process_handle();
    if ( proc )
	proc->set_last_report(new sc_report(rep));
    else
    {
	if ( last_global_report )
	    delete last_global_report;
	last_global_report = new sc_report(rep);
    }
}

//
// backward compatibility with 2.0+
//

sc_msg_def * sc_report_handler::mdlookup(int id)
{
    for ( msg_def_items * item = messages; item; item = item->next )
    {
	for ( int i = 0; i < item->count; ++i )
	    if ( id == item->md[i].id )
		return item->md + i;
    }
    return 0;
}

//
// CLASS: sc_report_handler
// static variables
//

sc_actions sc_report_handler::suppress_mask = 0;
sc_actions sc_report_handler::force_mask = 0;

sc_actions sc_report_handler::sev_actions[SC_MAX_SEVERITY] =
{
    /* info  */ SC_DEFAULT_INFO_ACTIONS,
    /* warn  */ SC_DEFAULT_WARNING_ACTIONS,
    /* error */ SC_DEFAULT_ERROR_ACTIONS,
    /* fatal */ SC_DEFAULT_FATAL_ACTIONS
};

sc_actions sc_report_handler::sev_limit[SC_MAX_SEVERITY] =
{
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX
};
sc_actions sc_report_handler::sev_call_count[SC_MAX_SEVERITY] = { 0, 0, 0, 0 };

sc_report* sc_report_handler::last_global_report = NULL;
sc_actions sc_report_handler::available_actions =
    SC_DO_NOTHING |
    SC_THROW |
    SC_LOG |
    SC_DISPLAY |
    SC_CACHE_REPORT |
    SC_INTERRUPT |
    SC_STOP |
    SC_ABORT;

sc_report_handler_proc sc_report_handler::handler =
    &sc_report_handler::default_handler;

char * sc_report_handler::log_file_name = 0;

sc_report_handler::msg_def_items * sc_report_handler::messages =
    &sc_report_handler::msg_terminator;


//
// predefined messages
//

const char SC_ID_REGISTER_ID_FAILED_[] = "register_id failed";
const char SC_ID_UNKNOWN_ERROR_[]      = "unknown error";
const char SC_ID_WITHOUT_MESSAGE_[]    = "";
const char SC_ID_NOT_IMPLEMENTED_[]    = "not implemented";
const char SC_ID_INTERNAL_ERROR_[]     = "internal error";
const char SC_ID_ASSERTION_FAILED_[]   = "assertion failed";
const char SC_ID_OUT_OF_BOUNDS_[]      = "out of bounds";

#define DEFINE_MSG(id,n)                                                     \
    {                                                                        \
	(id),                                                                \
	0u, {0u}, /* actions */                                              \
	0u, {0u}, 0u, /* limits */                                           \
	0u, {0u}, NULL, /* call counters */                                  \
	n                                                                    \
    }

static sc_msg_def default_msgs[] = {
    DEFINE_MSG(SC_ID_REGISTER_ID_FAILED_, 800),
    DEFINE_MSG(SC_ID_UNKNOWN_ERROR_, 0),
    DEFINE_MSG(SC_ID_WITHOUT_MESSAGE_, 1),
    DEFINE_MSG(SC_ID_NOT_IMPLEMENTED_, 2),
    DEFINE_MSG(SC_ID_INTERNAL_ERROR_, 3),
    DEFINE_MSG(SC_ID_ASSERTION_FAILED_, 4),
    DEFINE_MSG(SC_ID_OUT_OF_BOUNDS_, 5)
};

sc_report_handler::msg_def_items sc_report_handler::msg_terminator =
{
    default_msgs,
    sizeof(default_msgs)/sizeof(*default_msgs),
    false,
    NULL
};

} // namespace sc_core

// Taf!
