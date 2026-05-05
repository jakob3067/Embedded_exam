#include "emp_type.h"

#define MAX_MSG 64

INT8U   msg_event[MAX_MSG];
INT8U   msg_state[MAX_MSG];

INT8U get_msg_event( no )
INT8U no;
{
  INT8U result = 0;

  if( no < MAX_MSG )
  {
    if( msg_event[no] )
	  {
	    result = msg_event[no];
	    msg_event[no] = 0;
	  }
  }
  return( result );
}

void put_msg_event( no, event )
INT8U  no;
INT8U  event;
{
  if( no < MAX_MSG )
    msg_event[no] = event;
}

INT8U get_msg_state( no )
INT8U no;
{
  INT8U result = 0;

  if( no < MAX_MSG )
  {
    result = msg_state[no];
  }
  return( result );
}

void put_msg_state( no, state )
INT8U  no;
INT8U  state;
{
  if( no < MAX_MSG )
    msg_state[no] = state;
}
