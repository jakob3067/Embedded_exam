#include "emp_type.h"
#include "sem.h"
#include "glob_def.h"

INT8U    semaphore[MAX_SEM];

BOOLEAN wait( INT8U sem_no )
{
  INT8U Result;
  
  if( sem_no < MAX_SEM )
  {
	  Result = semaphore[sem_no];
    semaphore[sem_no] = FALSE;
  }
  else
    Result = FALSE;
  return( Result );
}

void signal( INT8U sem_no )
{
  if( sem_no < MAX_SEM )
	  semaphore[sem_no] = TRUE;
}
