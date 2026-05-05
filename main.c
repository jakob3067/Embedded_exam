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

int main(void)
{
    // 1. Initialize hardware
    init_systick();
    init_gpio();
    
    // 2. Initialize the software queue (Mandatory!)
    // Make sure Q_OUTPUT is initialized so the pointers aren't null
    queue_open( Q_OUTPUT );

    // 3. Queue up your message before entering the loop
    // This puts characters into the buffer, but doesn't show them yet
    wr_str_LCD((INT8U *)"Hello World!");
    move_LCD(0, 1); // Move to second line
    wr_str_LCD((INT8U *)"No RTOS yet");

    while(1)
    {
        // 4. Run the LCD state machine
        // Each call processes one character from the queue
        lcd_task(0);

        // Optional: Toggle your LED
        // Note: Without a delay, this toggles so fast you won't see it
        GPIO_PORTD_DATA_R ^= 0x40;
        
        // 5. Add a small manual delay 
        // The LCD needs time between commands. Your wr_ctrl/out functions 
        // have loops, but a small delay here helps stability.
        volatile int i;
        for(i = 0; i < 10000000000000000000; i++);
    }
}
