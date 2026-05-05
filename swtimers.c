#include "emp_type.h"
#include "glob_def.h"
#include "swtimers.h"

#define NOF_SWTIMERS 8

typedef struct
{
    INT8U  running;
    INT16U counter;
    INT16U timeout;
} swtimer_t;

swtimer_t swtimer_pool[NOF_SWTIMERS];

void init_swtimers(void)
{
    INT8U i;
    for( i = 0; i < NOF_SWTIMERS; i++ )
    {
        swtimer_pool[i].running = FALSE;
        swtimer_pool[i].counter = 0;
        swtimer_pool[i].timeout = 0;
    }
}

void start_swtimer( INT8U timer_no, INT8U task_no, INT16U timeout )
{
    if( timer_no < NOF_SWTIMERS )
    {
        swtimer_pool[timer_no].running = TRUE;
        swtimer_pool[timer_no].counter = 0;
        swtimer_pool[timer_no].timeout = timeout;
    }
}

INT16U swtimer( INT8U timer_no )
{
    if( timer_no < NOF_SWTIMERS )
        if( swtimer_pool[timer_no].running )
            return swtimer_pool[timer_no].timeout - swtimer_pool[timer_no].counter;
    return 0;
}

void swt_ctrl(void)
{
    INT8U i;
    for( i = 0; i < NOF_SWTIMERS; i++ )
        if( swtimer_pool[i].running )
        {
            swtimer_pool[i].counter++;
            if( swtimer_pool[i].counter >= swtimer_pool[i].timeout )
                swtimer_pool[i].running = FALSE;
        }
}
