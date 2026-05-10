#include "emp_type.h"
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "tmodel.h"
#include "task.h"
#include "lcd.h"


/*****************************    Defines    *******************************/

#define HIB_LOAD_R (*((volatile uint32_t *)0x400FC00C))

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U sec, min, hour;



/*****************************   Functions   *******************************/
INT8U get_hour()
{
  return( hour );
}
INT8U get_min()
{
  return( min );
}
INT8U get_sec()
{
  return( sec );
}

void rtc_set_time( INT8U h, INT8U m, INT8U s )
{
  // Set the real-time clock in seconds
  INT32U total_seconds = (h * 3600) + (m * 60) + s;
  while (!(HIB_CTL_R & 0x80000000));
  HIB_LOAD_R = total_seconds;
  
}

void rtc_task(void *pvParameters)
{
  volatile INT32U i;

  SYSCTL_RCGCHIB_R |= 0x01; // Enable clock to Hibernation module

  vTaskDelay(pdMS_TO_TICKS(50));

  while (!(HIB_CTL_R & 0x80000000));
  if (!(HIB_CTL_R & 0x01))
  {
      HIB_CTL_R |= 0x00000040; // Tiva 32.768 kHz crystal
  }

  vTaskDelay(pdMS_TO_TICKS(100));

  while (!(HIB_CTL_R & 0x80000000));
  HIB_CTL_R |= 0x00000001;

  while (!(HIB_CTL_R & 0x80000000));
  HIB_LOAD_R = 0;

  while(1)
  {
    // Tiva RTC
    INT32U time = HIB_RTCC_R % 86400; // Wrap at 24 hours

    hour = time / 3600;
    min = (time % 3600) / 60;
    sec = time % 60;

    // Updates every second
    vTaskDelay( pdMS_TO_TICKS(1000) );
  }
}

/****************************** End Of Module *******************************/












