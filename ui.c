/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "ui.h"
#include "rtc.h"
#include "string.h"
#include "uart.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U i;
INT8U InBuf[128];

/*****************************   Functions   *******************************/


void ui_task(void *pvParameters)
{
  INT8U ch;
  while (1)
      {
          if (uart0_rx_rdy())
          {
              ch = uart0_getc();
              switch (ch)
              {
                  case 'r': /* send report */ break;
                  case 'p': /* set price */   break;
              }
          }
          vTaskDelay(pdMS_TO_TICKS(10));
      }
}

/****************************** End Of Module *******************************/












