#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "glob_def.h"
#include "emp_type.h"
#include "gpio.h"
#include "queue.h"
#include "tmodel.h"

extern volatile INT16S ticks;
INT16S alive_timer = MILLISEC(500);

int main(void)
{
	init_systick();
    init_gpio();

	while(1){

		GPIO_PORTD_DATA_R ^= 0x40;

		// Protected operating system mode
		swt_ctrl();

		// Application mode
		button_task( TASK_BUTTON );
		rtc_task( TASK_RTC );
		display_rtc_task( TASK_RTC_DISPLAY );
		ajust_rtc_task( TASK_RTC_ADJUST );
		lcd_task( TASK_LCD );
	}

	return( 0 );
}
