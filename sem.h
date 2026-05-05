#ifndef SEM_H_
#define SEM_H_

#include "emp_type.h"
#define MAX_SEM  8

BOOLEAN wait(INT8U);

void signal(INT8U);

#endif /*SEM_H_*/
