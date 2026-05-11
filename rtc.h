#ifndef RTC_H_
#define RTC_H_


INT8U get_hour();
INT8U get_min();
INT8U get_sec();

void rtc_task( void *pvParameters );
void rtc_init();
void rtc_set_time( INT8U h, INT8U m, INT8U s );

#endif /*RTC_H_*/
