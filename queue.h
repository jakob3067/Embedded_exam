#ifndef _QUEUE_H
#define _QUEUE_H

#include "emp_type.h"

INT8S queue_open( INT8U );
INT8U queue_put( INT8U, INT8U );
INT8U queue_get( INT8U );
INT8U queue_test( INT8U );

#endif
