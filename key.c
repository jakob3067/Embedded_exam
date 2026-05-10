/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "emp_type.h"
#include "tmodel.h"
#include "task.h"
#include "queue.h"
#include "lcd.h"
#include "gpio.h"
#include "controller.h"

/***************************** Defines    *******************************/
// External queues defined in main

extern QueueHandle_t xLCDQueue;
extern QueueHandle_t xKeyQueue;

/***************************** Functions   *******************************/

static INT8U row( INT8U y )
{
  INT8U result = 0;

  switch( y )
  {
    case 0x01: result = 1; break;
    case 0x02: result = 2; break;
    case 0x04: result = 3; break;
    case 0x08: result = 4; break;
  }
  return( result );
}

static INT8U key_catch(INT8U x, INT8U y )
{
  const INT8U matrix[3][4] = {{'*','7','4','1'},
                              {'0','8','5','2'},
                              {'#','9','6','3'}};

  if (x < 1 || x > 3 || y < 1 || y > 4) return 0;
  return( matrix[x-1][y-1] );
}

void drive_column(INT8U col)
{
    GPIO_PORTA_DATA_R |= 0x1C;

    switch(col)
    {
        case 1: GPIO_PORTA_DATA_R &= 0x04; break; // PA2
        case 2: GPIO_PORTA_DATA_R &= 0x08; break; // PA3
        case 3: GPIO_PORTA_DATA_R &= 0x10; break; // PA4
    }
}

BOOLEAN get_keyboard( INT8U *pch )
{
  //return( get_queue( xKeyQueue, pch, 20 ));
}

BOOLEAN check_column(INT8U x, INT8U *pressed_key)
{
    drive_column(x);

    volatile int i;
    for(i = 0; i < 100; i++);

    INT8U y = (~GPIO_PORTE_DATA_R) & 0x0F;          // Save the values of the 4 bits for the rows

    if( y )                                         // If one of them are set...
    {                                               // ...we first find the row number with the function row()
        INT8U r = row(y);
        if (r > 0)
        {
            *pressed_key = key_catch(x, r);
            return 1;
        }
    }
    return 0;
}

void key_init(void)
{
    volatile uint32_t dummy;
    // 1. Enable Clocks for Port A (Columns) and Port E (Rows)
    SYSCTL_RCGC2_R |= (SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOE);
    dummy = SYSCTL_RCGC2_R; // Allow time for clocks to stabilize

    // 2. Configure Port A (PA2, PA3, PA4) as Outputs for Columns
    GPIO_PORTA_DIR_R |= 0x1C;
    GPIO_PORTA_DEN_R |= 0x1C;

    // 3. Configure Port E (PE0, PE1, PE2, PE3) as Inputs for Rows
    GPIO_PORTE_DIR_R &= ~0x0F;
    GPIO_PORTE_DEN_R |= 0x0F;
    GPIO_PORTE_PDR_R |= 0x0F; // Enable Pull-down resistors to avoid floating pins
}

void key_task(void *pvParameters)
{
  INT8U column;
  INT8U debounce_count;
  
  (void)pvParameters;
  INT8U key_val = 0;
  INT8U col;


  while(1)
  {
      for(col = 1; col <= 3; col++)
      {
          if( check_column(col, &key_val) )
          {
              // FreeRTOS queue system
              xQueueSend( xKeyQueue, &key_val, 0);

              vTaskDelay(pdMS_TO_TICKS(250));

              GPIO_PORTF_DATA_R &= ~0xFD;
              vTaskDelay(pdMS_TO_TICKS(2050));
              break;
              }
      }
      vTaskDelay(pdMS_TO_TICKS(20)); // Polling rate
  }

  /* Continuous keypad scanning */
  while(1)
  {
    /* Scan each of the 3 columns */
    for(column = 1; column <= 3; column++)
    {
      /* Select column by setting the appropriate PORTC bit (4-6) */
      /* Column 1 = bit 4 (0x10), Column 2 = bit 5 (0x20), Column 3 = bit 6 (0x40) */
      GPIO_PORTC_DATA_R = (column << 4) & 0xF0;  /* Set column bits, clear others */
      
      /* Wait for column to stabilize */
      vTaskDelay(pdMS_TO_TICKS(5));
      
      /* Check if any key in this column is pressed */
      if(check_column(column))
      {
        /* Debounce: wait for key to settle */
        vTaskDelay(pdMS_TO_TICKS(20));
        
        /* Check again to confirm keypress */
        if(check_column(column))
        {
          /* Key confirmed pressed - wait for it to be released */
          vTaskDelay(pdMS_TO_TICKS(100));
          
          /* Wait for key release (all rows inactive) */
          debounce_count = 0;
          while((GPIO_PORTE_DATA_R & 0x0F) && debounce_count < 50)
          {
            vTaskDelay(pdMS_TO_TICKS(10));
            debounce_count++;
          }
        }
      }
      
      /* Short inter-column delay */
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}
