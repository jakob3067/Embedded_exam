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
#include "task.h"
#include "queue.h"
#include "emp_type.h"
#include "key.h"

/***************************** Defines    *******************************/
// External queues defined in main
extern QueueHandle_t xKeyQueue;
extern QueueHandle_t xLCDQueue;

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

static INT8U key_catch(INT8U x, INT8U y)
{
  const INT8U matrix[3][4] = {{'*','7','4','1'},
                              {'0','8','5','2'},
                              {'#','9','6','3'}};

  if (x < 1 || x > 3 || y < 1 || y > 4) return 0;
  return( matrix[x-1][y-1] );
}

void drive_column(INT8U col)
{
    GPIO_PORTA_DATA_R |= 0x1C;  /* Set all column bits */
    
    switch(col)
    {
        case 1: GPIO_PORTA_DATA_R &= 0xE7; break;  /* PA3 and PA4 high, PA2 low (column 1) */
        case 2: GPIO_PORTA_DATA_R &= 0xEF; break;  /* PA2 and PA4 high, PA3 low (column 2) */
        case 3: GPIO_PORTA_DATA_R &= 0xF7; break;  /* PA2 and PA3 high, PA4 low (column 3) */
    }
}

BOOLEAN get_keyboard( INT8U *pch )
{
  if ( xKeyQueue == NULL )
    return 0;

  if ( xQueueReceive( xKeyQueue, pch, pdMS_TO_TICKS(20) ) == pdTRUE )
    return 1;
  else
    return 0;
}

BOOLEAN check_column(INT8U x, INT8U *pressed_key)
{
    drive_column(x);
    
    volatile int i;
    for(i = 0; i < 100; i++);  /* Debounce delay */

    INT8U y = (~GPIO_PORTE_DATA_R) & 0x0F;  /* Save the values of the 4 bits for the rows */

    if( y )  /* If one of them are set... */
    {
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
    /* GPIO initialization is now handled by init_gpio() in gpio.c */
    /* This function is kept for compatibility but all setup is done centrally */
}

void key_task(void *pvParameters)
{
  (void)pvParameters;
  INT8U key_val = 0;
  INT8U col;
  INT8U key_pressed_flag = 0;

  while(1)
  {
      for(col = 1; col <= 3; col++)
      {
          if( check_column(col, &key_val) && !key_pressed_flag )
          {
              key_pressed_flag = 1;  /* Debounce: mark key as pressed */
              
              /* Send key to xKeyQueue */
              xQueueSend( xKeyQueue, &key_val, 0);

              /* Send the pressed key to the LCD as a short string */
              {
                  INT8U *pStr;
                  static INT8U keybuf[2];
                  keybuf[0] = key_val;
                  keybuf[1] = '\0';
                  pStr = keybuf;
                  xQueueSend( xLCDQueue, &pStr, portMAX_DELAY );
              }

              /* If 'e' pressed, light the status LED (PD6) for 500ms */
              if (key_val == 'e' || key_val == 'E')
              {
                  GPIO_PORTD_DATA_R |= 0x40;  /* PD6 on */
                  vTaskDelay(pdMS_TO_TICKS(500));
                  GPIO_PORTD_DATA_R &= ~0x40; /* PD6 off */
              }

              vTaskDelay(pdMS_TO_TICKS(250));
              break;
          }
      }

      /* Check if key is released */
      INT8U all_rows = GPIO_PORTE_DATA_R & 0x0F;
      if (all_rows == 0x0F && key_pressed_flag)  /* All row bits released (pull-down unpressed) */
      {
          key_pressed_flag = 0;  /* Reset debounce flag */
      }

      vTaskDelay(pdMS_TO_TICKS(20));  /* Polling rate */
  }
}
