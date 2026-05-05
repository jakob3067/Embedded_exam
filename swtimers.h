#ifndef _SWTIMERS_H
#define _SWTIMERS_H

#define TICK_TIME	5		// tick time in milli seconds.

#define MILLISEC(time_in_msec)  time_in_msec / TICK_TIME

INT16U swtimer( INT8U );

void start_swtimer( INT8U, INT8U, INT16U );

void swt_ctrl(void);

void init_swtimers(void);

#endif
