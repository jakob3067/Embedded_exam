#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "glob_def.h"
#include "emp_type.h"
#include "gpio.h"
#include "queue.h"
#include "tmodel.h"
#include "lcd.h"
#include "swtimers.h"
#include "events.h"
#include "sem.h"

int main(void)
{
    init_systick();
    init_gpio();
    
    signal( MUTEX_SYSTEM_RTC );
    signal( MUTEX_LCD_DISPLAY );
    signal( SEM_RTC_UPDATED );

    while(1)
    {
        // Protected operating system mode
        swt_ctrl();

        // Application mode
        lcd_task( TASK_LCD );
    }
    
}
