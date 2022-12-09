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

  sc_signal.h -- The sc_signal<T> primitive channel class.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:
    
 *****************************************************************************/

#ifndef SC_SIGNAL_H
#define SC_SIGNAL_H


#include "sysc/communication/sc_port.h"
#include "sysc/communication/sc_prim_channel.h"
#include "sysc/communication/sc_signal_ifs.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_process_b.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/datatypes/bit/sc_logic.h"
#include "sysc/tracing/sc_trace.h"
#include "sysc/utils/sc_string.h"
#include <typeinfo>

namespace sc_core {

// to avoid code bloat in sc_signal<T>

extern
void
sc_signal_invalid_writer( const char* name,
			  const char* kind,
			  const char* first_writer,
			  const char* second_writer );


// ----------------------------------------------------------------------------
//  CLASS : sc_signal<T>
//
//  The sc_signal<T> primitive channel class.
// ----------------------------------------------------------------------------

template <class T>
class sc_signal
: public sc_signal_inout_if<T>,
  public sc_prim_channel
{
public:

    // constructors

    sc_signal()
	: sc_prim_channel( sc_gen_unique_name( "signal" ) ),
          m_output( 0 ), m_cur_val( T() ), m_new_val( T() ),
          m_delta( ~sc_dt::UINT64_ONE ), m_writer( 0 )
	{}

    explicit sc_signal( const char* name_ )
	: sc_prim_channel( name_ ),
          m_output( 0 ), m_cur_val( T() ), m_new_val( T() ),
          m_delta( ~sc_dt::UINT64_ONE ), m_writer( 0 )
	{}


    // destructor (does nothing)

    virtual ~sc_signal()
	{}


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ return m_value_changed_event; }


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ return m_value_changed_event; }


    // read the current value
    virtual const T& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const T& get_data_ref() const
        { return m_cur_val; }


    // was there an event?
    virtual bool event() const
        { return ( simcontext()->delta_count() == m_delta + 1 ); }

    // write the new value
    virtual void write( const T& );


    // other methods

    operator const T& () const
	{ return read(); }


    sc_signal<T>& operator = ( const T& a )
	{ write( a ); return *this; }

    sc_signal<T>& operator = ( const sc_signal<T>& a )
	{ write( a.read() ); return *this; }


    const T& get_new_value() const
	{ return m_new_val; }


    void trace( sc_trace_file* tf ) const
#ifdef DEBUG_SYSTEMC
	{ sc_trace( tf, get_data_ref(), name() ); }
#else
	{}
#endif


    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "sc_signal"; }

protected:

    virtual void update();

    void check_writer();

protected:

    sc_port_base* m_output; // used for static design rule checking

    T             m_cur_val;
    T             m_new_val;

    sc_event      m_value_changed_event;

    sc_dt::uint64 m_delta; // delta of last event

    sc_process_b* m_writer; // used for dynamic design rule checking

private:

    // disabled
    sc_signal( const sc_signal<T>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII


template <class T>
inline
void
sc_signal<T>::register_port( sc_port_base& port_, const char* if_typename_ )
{
#ifdef DEBUG_SYSTEMC
    std::string nm( if_typename_ );
    if( nm == typeid( sc_signal_inout_if<T> ).name() ) {
	// an out or inout port; only one can be connected
	if( m_output != 0 ) {
	    sc_signal_invalid_writer( name(), kind(),
				      m_output->name(), port_.name() );
	}
	m_output = &port_;
    }
#endif
}


// write the new value

template <class T>
inline
void
sc_signal<T>::write( const T& value_ )
{
#ifdef DEBUG_SYSTEMC
    check_writer();
#endif
    m_new_val = value_;
    if( !( m_new_val == m_cur_val ) ) {

	++simcontext()->signal_write;

	request_update();
    }
}

	
template <class T>
inline
void
sc_signal<T>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

template <class T>
inline
void
sc_signal<T>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
    os << "new value = " << m_new_val << ::std::endl;
}



template <class T>
inline
void
sc_signal<T>::update()
{
	
	


	//printf("Fault Duration_time : %d \n", fault_duration_time);
	int permanent_enroll_set =0;

	
	int selected_location=-1; 
	int peramenent_list_search =-1;
	int temp_set =-1;

	int pre_list_enroll_set =1;
	int sel_location = -1;

	if(simcontext()->Simcontext_FCDS.fault_enable == true)											// d fault injection; ���� ���˻�
	{



		

		if( simcontext()->fault_set == true)							// fault injection time  Ǿ��˻�
		{
	
		//	printf("simcontext()->signal_write : %d, simcontext()->select_signal %d\n", simcontext()->signal_write, simcontext()->select_signal);
			if(simcontext()->Simcontext_FCDS.fault_config_type == 2 || simcontext()->Simcontext_FCDS.fault_config_type == 4)
			{

				//printf("DDDDDDDDDDDDD %d\n", simcontext()->select_signal);
				simcontext()->select_signal --;
			}
		//	printf("signal updata : %s\n",name());

			if(simcontext()->fault_location_set == false && simcontext()->permanent_l.enable == 0)				// fault injection� ߻Ǿ� 'ġ ��			
			{															// d��ȴ�
				//srand(time(NULL));
				//	printf("********************************\n");	

				if(simcontext()->fault_data <= 2)
				{
					simcontext()->fault_data = 2;
				}
					

				if(simcontext()->fault_signal_sel == 0)
				{
					simcontext()->rand_signal_select = 0;
				}
				else
				{
					simcontext()->rand_signal_select = rand() % simcontext()->fault_signal_sel;		// � signal �� �� ����'� 1/20�� Ȯ; ο��
				}

				
				if(simcontext()->select_signal ==0 || simcontext()->select_signal <0 )//simcontext()->fault_signal_sel==0)
				{
					

					if(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 1)
					{	// transient fualt � ...........

						printf("A1 fault signal event : %s\n",name());

						simcontext()->fault_data=rand() % m_new_val.m_len;					// fault  ߻Ǵ�bit d
					}
					else if( simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 2)
					{	// permanent fault � ...........fault list � ����
						// random ű�...

						strcpy(simcontext()->permanent_l.name[simcontext()->permanent_l.current_list_num], name());
						simcontext()->fault_data=rand() % m_new_val.m_len;
						simcontext()->permanent_l.fualt_bit[simcontext()->permanent_l.current_list_num] = simcontext()->fault_data;
						
						simcontext()->permanent_l.current_list_num++;
						simcontext()->permanent_l.enable =1;
						permanent_enroll_set = 1;
						simcontext()->set = 0;

						simcontext()->select_signal =0;
							
						
						printf("A2 fault signal event : %s \n",name());

					}
					else
					{
						// ����= 
					}

					

				}

			}
			else if(simcontext()->fault_location_set == false && simcontext()->permanent_l.enable == 1)
			{

				if(simcontext()->set == 1)
				{


						//srand(time(NULL));
						//	printf("********************************\n");	

						if(simcontext()->fault_data <= 2)
						{
							simcontext()->fault_data = 2;
						}
							

						if(simcontext()->fault_signal_sel == 0)
						{
							simcontext()->rand_signal_select = 0;
						}
						else
						{
							simcontext()->rand_signal_select = rand() % simcontext()->fault_signal_sel;		// � signal �� �� ����'� 1/20�� Ȯ; ο��
						}

						
						if(simcontext()->select_signal ==0 || simcontext()->select_signal <0 )//simcontext()->fault_signal_sel==0)
						{
							// permanent fault list � � 

							for(int m=0; m < simcontext()->permanent_l.current_list_num; m++)
							{
								sel_location = strcmp(simcontext()->permanent_l.name[m],name());

								if(sel_location == 0)
								{
									pre_list_enroll_set =0;
									simcontext()->select_signal = 10;		// � delay
									//break;	// � List �� Ǿ��8�� � � �� �	
								}
							}


							if(pre_list_enroll_set != 0)		// ο�Permanent fault � 
							{
								strcpy(simcontext()->permanent_l.name[simcontext()->permanent_l.current_list_num], name());
								simcontext()->fault_data=rand() % m_new_val.m_len;
								simcontext()->permanent_l.fualt_bit[simcontext()->permanent_l.current_list_num] = simcontext()->fault_data;
								
								simcontext()->permanent_l.current_list_num++;
								permanent_enroll_set = 1;

								simcontext()->select_signal =0;
								printf("E2 fault signal event : %s \n",name());

							}

							simcontext()->set = 0;



						}
			

				}
				else
				{	// ű��� ƴϸ� � Ʈ �Ǿ���7�� Ȯ� � active ��
					for(int n=0; n < simcontext()->permanent_l.current_list_num; n++)
					{
						temp_set = strcmp(simcontext()->permanent_l.name[n], name());
						

						if(temp_set == 0)
						{
							simcontext()->select_signal =0;
							
							//printf("Step 111  %s : %d =? %s \n", name(), n, simcontext()->permanent_l.name[0]);
							peramenent_list_search =0;		// �� updata Ǵ�� permanent list �Ǿ���
							simcontext()->fault_data = simcontext()->permanent_l.fualt_bit[n];
							//printf("D2 fault signal event : %s & time : %d\n",name(), simcontext()->m_curr_time);
						}
					}

				}

				


			}
			else if(simcontext()->fault_location_set == true && 
				!strcmp(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name())
				&& simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 1)		// 7�� 'ġ deterministic ���Ǿ�� �
			{
					printf("B fault signal event : %s \n",name());
					//printf(" %d \n", simcontext()->Simcontext_FCDS.current_compound);
//					int a;
//					scanf("%d", &a);
					simcontext()->select_signal =0;
					simcontext()->fault_data=rand() % m_new_val.m_len;					// fault  ߻Ǵ�bit d
					simcontext()->permanent_l.fualt_bit[simcontext()->permanent_l.current_list_num] = simcontext()->fault_data;					

			}
			else if(simcontext()->permanent_l.enable == 0 && simcontext()->fault_location_set == true &&
				!strcmp(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name())
				&& simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 2 )
			{
				// �� permanent fault list  

				strcpy(simcontext()->permanent_l.name[simcontext()->permanent_l.current_list_num], name());
				simcontext()->fault_data=rand() % m_new_val.m_len;
				simcontext()->permanent_l.fualt_bit[simcontext()->permanent_l.current_list_num] = simcontext()->fault_data;
				
				simcontext()->permanent_l.current_list_num++;
				simcontext()->permanent_l.enable =1;
				permanent_enroll_set = 1;
				simcontext()->set = 0;

				simcontext()->select_signal =0;
					
				
				printf("C fault signal event : %s\n",name());


			}
			else if(simcontext()->permanent_l.enable == 1)
			{
				// �Ǵ�permanent fault list � 
					// ����fault � Ʈ ���
					for(int n=0; n < simcontext()->permanent_l.current_list_num; n++)
					{
						temp_set = strcmp(simcontext()->permanent_l.name[n], name());
						

						if(temp_set == 0)
						{
							simcontext()->select_signal =0;
							
							//printf("Step 111  %s : %d =? %s \n", name(), n, simcontext()->permanent_l.name[0]);
							peramenent_list_search =0;		// �� updata Ǵ�� permanent list �Ǿ���
							simcontext()->fault_data = simcontext()->permanent_l.fualt_bit[n];
							//printf("D1 fault signal event : %s & time : %d\n",name(), simcontext()->m_curr_time);
						}
					}

					if(!strcmp(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name()))
					{
						// � ߵ�fault�fault list � ��




						if(simcontext()->set == 1)				// �� ο�permanent fualt  ���. 
						{


							for(int m=0; m < simcontext()->permanent_l.current_list_num; m++)
							{
								sel_location = strcmp(simcontext()->permanent_l.name[m],name());

								if(sel_location == 0)
								{
									pre_list_enroll_set =0;
									//break;	// � List �� Ǿ��8�� � � �� �	
								}
							}


							if(pre_list_enroll_set != 0)		// ο�Permanent fault � 
							{
								strcpy(simcontext()->permanent_l.name[simcontext()->permanent_l.current_list_num], name());
								simcontext()->fault_data=rand() % m_new_val.m_len;
								simcontext()->permanent_l.fualt_bit[simcontext()->permanent_l.current_list_num] = simcontext()->fault_data;
								
								simcontext()->permanent_l.current_list_num++;
								permanent_enroll_set = 1;

								simcontext()->select_signal =0;
								printf("E1 fault signal event : %s\n",name());

							}

							simcontext()->set = 0;
						}
					}					

			}
			else
			{
				//printf("ZZZZZZZZZZZZZZz :%s:\n", 
				//	simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location);

					simcontext()->select_signal =1;
			}
			//printf("AAAAAAAAAAA\n");
			//printf("%d   %d    %s     %s \n", simcontext()->m_curr_time, simcontext()->fault_location_set, 
			//			simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,
			//			name());
		///////////////////////////////////////////////////////////////////////////////

			if(simcontext()->select_signal ==0 || simcontext()->select_signal <0)//simcontext()->fault_signal_sel==0)
			{
				simcontext()->select_signal =1;	// : reset



				if(simcontext()->fault_location_set == false)
				{
					strcpy(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name());

					//strcpy(simcontext()->per_signal,"voter_im_out");

					simcontext()->fault_location_set = true;
				}
//printf("BBBBBBBBbBBB %s \n", name());
				//printf("signal %s \n", name());
				
				// prinmariy fault �compound fault � � ��Data � ÷� з ��ϴ�
				//printf("****************************************  %d \n", simcontext()->Simcontext_FCDS.Data[0].fault_type);
				if(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 1)			// Transient fault
				{

					if(!strcmp(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name()))
					{
						simcontext()->fault_set = false;
					}

				}
				else if(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 2)		// Permanent fault
				{



					//printf(" Permanent list enroll ***************** %d\n", simcontext()->permanent_l.current_list_num);

				}
				else
				{


				}

				


				// ��7�� 'ġ ��񱳱 (compound fault ����̱�)


				// transient fault ����7 ��Ǳ�, 7�� �� � ��˻絵 � �� ��
				//if(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 1)
				//{
					selected_location = strcmp(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_location,name());
				//}

				// permanent fault �� ��8�7 ��Ǿ�� ˻�Ѵ�
				//if(simcontext()->permanent_l.enable == 1)
				//{


				//}

	
				//int ddd = strcmp(simcontext()->per_signal,"voter_im_out");
				if(selected_location==0 || peramenent_list_search ==0)
				{


					if(simcontext()->Simcontext_FCDS.fault_config_type == 3 || 
						simcontext()->Simcontext_FCDS.fault_config_type == 4)
					{
						if(simcontext()->Simcontext_FCDS.Data[simcontext()->Simcontext_FCDS.current_compound].fault_type == 1)
						{
							if(simcontext()->Simcontext_FCDS.compound_num-1 > simcontext()->Simcontext_FCDS.current_compound)
							{
								//printf("CCCCCCCCCCCCCCCCCCCCC\n");
								simcontext()->Simcontext_FCDS.current_compound++;		// 
								simcontext()->crunch_set = false;
								simcontext()->set = false;
							}
						}

						if(permanent_enroll_set ==1)
						{
							permanent_enroll_set = 0;
							simcontext()->Simcontext_FCDS.current_compound++;

						}

						if(simcontext()->Simcontext_FCDS.fault_config_type == 4)
						{
							simcontext()->fault_location_set = false;
						}
					}

				//	printf("fault bit : %d   fault location : %s \n",simcontext()->fault_data , simcontext()->per_signal);
				//int i;
				//scanf("%d", &i);

					// prinmariy fault �compound fault � � ��Data � ÷� з ��ϴ�
					if(simcontext()->Simcontext_FCDS.Data[0].fault_value ==1) //// 1bit 0..////////////////////////////////////////////////////////////
					{
//						cout << "Original data : " << m_new_val.m_data[0] << endl;
						
						unsigned long temp = 0xfffffffe;
						for(int c =0 ; c < simcontext()->fault_data ; c++)
						{									//0 ~ 31
							temp = temp << 1;
							temp += 0x00000001;// >> 1;
						}
						m_new_val.m_data[0] &= temp;
					
						//printf("fault signal event : %s\n",name());
						//cout << "Fault Injected data : " << m_new_val.m_data[0] << " , Fault Location : " << 
						//	simcontext()->fault_data <<endl;
						
//						int a;
//						scanf("%d", &a);


					}
					else if(simcontext()->Simcontext_FCDS.Data[0].fault_value ==2) //// 1bit 1..////
					{
//						cout << "Original data : " << m_new_val.m_data[0] << endl;

						unsigned long temp = 0;
						if(simcontext()->fault_data ==0){							
								temp += 1;
						}
						else
						{
							temp += 1;
							for(int c =0 ; c < simcontext()->fault_data ; c++)
							{
								temp *= 2;
							}
						}

						m_new_val.m_data[0] |= temp;

						//printf("fault signal event : %s\n",name());	
						//cout << "Fault Injected data : " << m_new_val.m_data[0] << " , Fault Location : " << 
						//	simcontext()->fault_data <<endl;
					
//						int a;
//						scanf("%d", &a);

					}
/*
					else if(fault_type_2 == 2)	//// muti bit 0..///
					{
//						cout << "Original data : " << m_new_val.m_data[0] << endl;
						unsigned long temp = 0;
						unsigned long temp1 = 0;
						if(simcontext()->fault_data ==0)
						{
							m_new_val.m_data[0] &= 0xfffffffe;
						}
						else if(simcontext()->fault_data ==1)
						{
							m_new_val.m_data[0] &= 0xfffffffc;
						}
						else
						{
							temp = 0xfffffffe;
							temp1 = 0xfffffffe;
							for(int c =0 ; c < simcontext()->fault_data ; c++)
							{									//0 ~ 31
								temp = temp << 1;
								temp += 0x00000001;// >> 1;
							}
							
							for(int c =0 ; c < simcontext()->fault_data-1 ; c++)
							{									//0 ~ 31
								temp1 = temp1 << 1;
								temp1 += 0x00000001;// >> 1;
							}
							temp = temp & temp1;	
								
							m_new_val.m_data[0] &= temp;

						}

//						cout << "Fault Injected data : " << m_new_val.m_data[0] << " , Fault Location : " << 
//							simcontext()->fault_data <<endl;
						
//						int a;
//						scanf("%d", &a);
						//To Do..
					}
					else if(fault_type_2 == 3) //// muti bit 1..///
					{
				//		cout << "Original data : " << m_new_val.m_data[0] << endl;
						unsigned long temp = 0;
						unsigned long temp1 = 0;
						if(simcontext()->fault_data ==0)
						{
							m_new_val.m_data[0] = 1;
						}
						else if(simcontext()->fault_data ==1)
						{
							m_new_val.m_data[0] = 3;
						}
						else
						{

							unsigned long temp = 1;
							unsigned long temp1 = 1;
							for(int c =0 ; c < simcontext()->fault_data ; c++)
							{
								temp *= 2;
							}
							
							for(int c =0 ; c < simcontext()->fault_data-1 ; c++)
							{
								temp1 *= 2;
							}

							temp = temp | temp1;
							m_new_val.m_data[0] |= temp;
						}
						
				//		cout << "Fault Injected data : " << m_new_val.m_data[0] << " , Fault Location : " << 
				//			simcontext()->fault_data <<endl;
						
				//		int a;
				//		scanf("%d", &a);

						
						//To Do..
						
					}
					*/
					else
					{
						;
					}

											
				}
			}
		
		///////////////////////////////////////////////////////////////////////////////
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//printf( "%s, size : %d,  data : %d, len : %d \n",  name(), m_cur_val.size, m_new_val.m_data[0], m_new_val.m_len);

//	m_new_val.m_data[0] = 4;

//    printf("%d signal updata : %s  : %d  : time %d \n", simcontext()->signal_bindo ,name(), simcontext()->signal_write, simcontext()->m_curr_time );	/////
//	++simcontext()->signal_bindo;

    if( !( m_new_val == m_cur_val ) ) {


	m_cur_val = m_new_val;
	m_value_changed_event.notify_delayed();
	m_delta = simcontext()->delta_count();
    }
}


template <class T>
inline
void
sc_signal<T>::check_writer()
{
    sc_process_b* writer = sc_get_curr_process_handle();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	sc_signal_invalid_writer( name(), kind(),
				  m_writer->name(), writer->name() );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : sc_signal<bool>
//
//  Specialization of sc_signal<T> for type bool.
// ----------------------------------------------------------------------------

template <>
class sc_signal<bool>
: public sc_signal_inout_if<bool>,
  public sc_prim_channel 
{
public:

    // constructors

    sc_signal()
	: sc_prim_channel( sc_gen_unique_name( "signal" ) ),
          m_output( 0 ),
          m_cur_val( false ),
          m_new_val( false ),
          m_delta( ~sc_dt::UINT64_ONE ),
          m_writer( 0 )
	{}

    explicit sc_signal( const char* name_ )
	: sc_prim_channel( name_ ),
          m_output( 0 ),
          m_cur_val( false ),
          m_new_val( false ),
          m_delta( ~sc_dt::UINT64_ONE ),
          m_writer( 0 )
	{}


    // destructor (does nothing)

    virtual ~sc_signal()
	{}


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ return m_value_changed_event; }


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ return m_value_changed_event; }

    // get the positive edge event
    virtual const sc_event& posedge_event() const
	{ return m_posedge_event; }

    // get the negative edge event
    virtual const sc_event& negedge_event() const
	{ return m_negedge_event; }


    // read the current value
    virtual const bool& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const bool& get_data_ref() const
        { return m_cur_val; }


    // was there a value changed event?
    virtual bool event() const
        { return ( simcontext()->delta_count() == m_delta + 1 ); }

    // was there a positive edge event?
    virtual bool posedge() const
	{ return ( event() && m_cur_val ); }

    // was there a negative edge event?
    virtual bool negedge() const
	{ return ( event() && ! m_cur_val ); }


    // write the new value
    virtual void write( const bool& );


    // delayed evaluation
    virtual const sc_signal_bool_deval& delayed() const;

    // other methods

    operator const bool& () const
	{ return read(); }


    sc_signal<bool>& operator = ( const bool& a )
	{ write( a ); return *this; }

    sc_signal<bool>& operator = ( const sc_signal<bool>& a )
	{ write( a.read() ); return *this; }


    const bool& get_new_value() const
	{ return m_new_val; }


    void trace( sc_trace_file* tf ) const
#ifdef DEBUG_SYSTEMC
	{ sc_trace( tf, get_data_ref(), name() ); }
#else
	{}
#endif


    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "sc_signal"; }

protected:

    virtual void update();

    void check_writer();

    virtual bool is_clock() const { return false; }

protected:

    sc_port_base* m_output; // used for static design rule checking

    bool          m_cur_val;
    bool          m_new_val;

    sc_event      m_value_changed_event;
    sc_event      m_posedge_event;
    sc_event      m_negedge_event;

    sc_dt::uint64 m_delta; // delta of last event

    sc_process_b* m_writer; // used for dynamic design rule checking

private:

    // disabled
    sc_signal( const sc_signal<bool>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
void
sc_signal<bool>::register_port( sc_port_base& port_, const char* if_typename_ )
{
#ifdef DEBUG_SYSTEMC
    std::string nm( if_typename_ );
    if( nm == typeid( sc_signal_inout_if<bool> ).name() ) {
	// an out or inout port; only one can be connected
	if( m_output != 0 ) {
	    sc_signal_invalid_writer( name(), kind(),
				      m_output->name(), port_.name() );
	}
	m_output = &port_;
    }
#endif
}


// write the new value

inline
void
sc_signal<bool>::write( const bool& value_ )
{
#ifdef DEBUG_SYSTEMC
    check_writer();
#endif
    m_new_val = value_;
    if( !( m_new_val == m_cur_val ) ) {
	request_update();
	
//	int i;
//	scanf("%d", &i);
	
//	++simcontext()->signal_write;
    }
}


// delayed evaluation

inline
const sc_signal_bool_deval&
sc_signal<bool>::delayed() const
{
    const sc_signal_in_if<bool>* iface = this;
    return RCAST<const sc_signal_bool_deval&>( *iface );
}


inline
void
sc_signal<bool>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

inline
void
sc_signal<bool>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
    os << "new value = " << m_new_val << ::std::endl;
}


inline
void
sc_signal<bool>::update()
{
    if( !( m_new_val == m_cur_val ) ) {
	m_cur_val = m_new_val;
	m_value_changed_event.notify_delayed();
	if( m_cur_val ) {
	    m_posedge_event.notify_delayed();
	} else {
	    m_negedge_event.notify_delayed();
	}
	m_delta = simcontext()->delta_count();

    }
}


inline
void
sc_signal<bool>::check_writer()
{
    if (is_clock()) return;
    sc_process_b* writer = sc_get_curr_process_handle();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	sc_signal_invalid_writer( name(), kind(),
				  m_writer->name(), writer->name() );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : sc_signal<sc_dt::sc_logic>
//
//  Specialization of sc_signal<T> for type sc_dt::sc_logic.
// ----------------------------------------------------------------------------

template <>
class sc_signal<sc_dt::sc_logic>
: public sc_signal_inout_if<sc_dt::sc_logic>,
  public sc_prim_channel
{
public:

    // constructors

    sc_signal()
	: sc_prim_channel( sc_gen_unique_name( "signal" ) ),
          m_output( 0 ),
	  m_cur_val(),
	  m_new_val(),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_writer( 0 )
	{}

    explicit sc_signal( const char* name_ )
	: sc_prim_channel( name_ ),
          m_output( 0 ),
	  m_cur_val(),
	  m_new_val(),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_writer( 0 )
	{}


    // destructor (does nothing)

    virtual ~sc_signal()
	{}


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ return m_value_changed_event; }


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ return m_value_changed_event; }

    // get the positive edge event
    virtual const sc_event& posedge_event() const
	{ return m_posedge_event; }

    // get the negative edge event
    virtual const sc_event& negedge_event() const
	{ return m_negedge_event; }


    // read the current value
    virtual const sc_dt::sc_logic& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const sc_dt::sc_logic& get_data_ref() const
        { return m_cur_val; }


    // was there an event?
    virtual bool event() const
        { return ( simcontext()->delta_count() == m_delta + 1 ); }

    // was there a positive edge event?
    virtual bool posedge() const
	{ return ( event() && m_cur_val == sc_dt::SC_LOGIC_1 ); }

    // was there a negative edge event?
    virtual bool negedge() const
	{ return ( event() && m_cur_val == sc_dt::SC_LOGIC_0 ); }


    // write the new value
    virtual void write( const sc_dt::sc_logic& );


    // delayed evaluation
    virtual const sc_signal_logic_deval& delayed() const;


    // other methods

    operator const sc_dt::sc_logic& () const
	{ return read(); }


    sc_signal<sc_dt::sc_logic>& operator = ( const sc_dt::sc_logic& a )
	{ write( a ); return *this; }

    sc_signal<sc_dt::sc_logic>& operator = (const sc_signal<sc_dt::sc_logic>& a)
	{ write( a.read() ); return *this; }


    const sc_dt::sc_logic& get_new_value() const
	{ return m_new_val; }


    void trace( sc_trace_file* tf ) const
#ifdef DEBUG_SYSTEMC
	{ sc_trace( tf, get_data_ref(), name() ); }
#else
	{}
#endif


    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "sc_signal"; }

protected:

    virtual void update();

    void check_writer();

protected:

    sc_port_base* m_output; // used for static design rule checking

    sc_dt::sc_logic      m_cur_val;
    sc_dt::sc_logic      m_new_val;

    sc_event             m_value_changed_event;
    sc_event             m_posedge_event;
    sc_event             m_negedge_event;

    sc_dt::uint64 m_delta; // delta of last event

    sc_process_b* m_writer; // used for dynamic design rule checking

private:

    // disabled
    sc_signal( const sc_signal<sc_dt::sc_logic>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
void
sc_signal<sc_dt::sc_logic>::register_port( sc_port_base& port_,
				    const char* if_typename_ )
{
#ifdef DEBUG_SYSTEMC
    std::string nm( if_typename_ );
    if( nm == typeid( sc_signal_inout_if<sc_dt::sc_logic> ).name() ) {
	// an out or inout port; only one can be connected
	if( m_output != 0 ) {
	    sc_signal_invalid_writer( name(), kind(),
				      m_output->name(), port_.name() );
	}
	m_output = &port_;
    }
#endif
}


// write the new value

inline
void
sc_signal<sc_dt::sc_logic>::write( const sc_dt::sc_logic& value_ )
{
#ifdef DEBUG_SYSTEMC
    check_writer();
#endif
    m_new_val = value_;
    if( !( m_new_val == m_cur_val ) ) {
	request_update();
	
	//++simcontext()->signal_write;
    }
}


// delayed evaluation

inline
const sc_signal_logic_deval&
sc_signal<sc_dt::sc_logic>::delayed() const
{
    const sc_signal_in_if<sc_dt::sc_logic>* iface = this;
    return RCAST<const sc_signal_logic_deval&>( *iface );
}


inline
void
sc_signal<sc_dt::sc_logic>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

inline
void
sc_signal<sc_dt::sc_logic>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
    os << "new value = " << m_new_val << ::std::endl;
}


inline
void
sc_signal<sc_dt::sc_logic>::update()
{
    if( !( m_new_val == m_cur_val ) ) {
	m_cur_val = m_new_val;
	m_value_changed_event.notify_delayed();
	if( m_cur_val == sc_dt::SC_LOGIC_1 ) {
	    m_posedge_event.notify_delayed();
	} else if( m_cur_val == sc_dt::SC_LOGIC_0 ) {
	    m_negedge_event.notify_delayed();
	}
	m_delta = simcontext()->delta_count();
    }
}



inline
void
sc_signal<sc_dt::sc_logic>::check_writer()
{
    sc_process_b* writer = sc_get_curr_process_handle();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	sc_signal_invalid_writer( name(), kind(),
				  m_writer->name(), writer->name() );
    }
}


// ----------------------------------------------------------------------------

template <class T>
inline
::std::ostream&
operator << ( ::std::ostream& os, const sc_signal<T>& a )
{
    return ( os << a.read() );
}

} // namespace sc_core

#endif

// Taf!
