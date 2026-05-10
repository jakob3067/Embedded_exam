/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
#include "tmodel.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "sem.h"
#include "queue.h"
#include "coffee.h"


/*****************************    Defines    *******************************/

#define QUEUE_LEN   128
extern QueueHandle_t xLCDQueue;

enum LCD_states
{
  LCD_POWER_UP,
  LCD_INIT,
  LCD_READY,
  LCD_ESC_RECEIVED,
};

/*****************************   Constants   *******************************/
const INT8U LCD_init_sequense[]= 
{
  0x30,		// Reset
  0x30,		// Reset
  0x30,		// Reset
  0x20,		// Set 4bit interface
  0x28,		// 2 lines Display
  0x0C,		// Display ON, Cursor OFF, Blink OFF
  0x06,		// Cursor Increment
  0x01,		// Clear Display
  0x02,     	// Home
  0xFF		// stop
}; 

/*****************************   Variables   *******************************/
enum LCD_states LCD_state = LCD_POWER_UP;
INT8U LCD_init;
static INT8U Mode4bit = FALSE;

/*****************************   Functions   *******************************/
void wr_str_LCD( INT8U *pStr )
{
  while (*pStr)
  {
    out_LCD(*pStr);
    vTaskDelay(pdMS_TO_TICKS(20));
    pStr++;
  }
}

void move_LCD( INT8U x, INT8U y )
{
  INT8U Pos;
  Pos = y*0x40 + x;
  Pos |= 0x80;
  out_LCD( ESC );  // was wr_ch_LCD(ESC)
  out_LCD( Pos );  // was wr_ch_LCD(Pos)
}

void wr_ctrl_LCD_low( INT8U Ch )
{
  INT8U temp;
  volatile int i;
  
  temp = GPIO_PORTC_DATA_R & 0x0F;
  temp  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTC_DATA_R  = temp;
  for( i=0; i<1000; i )
	  i++;
  GPIO_PORTD_DATA_R &= 0xFB;        // Select Control mode, write
  for( i=0; i<1000; i )
	  i++;
  GPIO_PORTD_DATA_R |= 0x08;		// Set E High

  for( i=0; i<1000; i )
	  i++;

  GPIO_PORTD_DATA_R &= 0xF7;		// Set E Low

  for( i=0; i<1000; i )
	  i++;
}

void wr_ctrl_LCD_high( INT8U Ch )
{
  wr_ctrl_LCD_low(( Ch & 0xF0 ) >> 4 );
}

void out_LCD_low( INT8U Ch )
{
  INT8U temp;
  volatile int i;
	  
  temp = GPIO_PORTC_DATA_R & 0x0F;
  GPIO_PORTC_DATA_R  = temp | ((Ch & 0x0F) << 4);
  //GPIO_PORTD_DATA_R &= 0x7F;        // Select write
  GPIO_PORTD_DATA_R |= 0x04;        // Select data mode
  for( i=0; i<1000; i )
        i++;
  GPIO_PORTD_DATA_R |= 0x08;		// Set E High
  for( i=0; i<1000; i )
        i++;
  GPIO_PORTD_DATA_R &= 0xF7;		// Set E Low
  for( i=0; i<1000; i )
        i++;
}

void out_LCD_high( INT8U Ch )
{
  out_LCD_low((Ch & 0xF0) >> 4);
}

void wr_ctrl_LCD( INT8U Ch )
{
  INT16U i;

  wr_ctrl_LCD_high( Ch );
  if( Mode4bit )
  {
    for(i=0; i<1000; i++);
    wr_ctrl_LCD_low( Ch );
  }
  else
  {
    if( (Ch & 0x30) == 0x20 )
      Mode4bit = TRUE;
  }
}

void clr_LCD()
{
  wr_ctrl_LCD( 0x01 );
  vTaskDelay(pdMS_TO_TICKS(5)); /* allow clear to be processed (~5ms) */
}


void home_LCD()
{
  wr_ctrl_LCD( 0x02 );
}

void Set_cursor( INT8U Ch )
{
  wr_ctrl_LCD( Ch );
}


void out_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  INT16U i;

  out_LCD_high( Ch );
  for(i=0; i<1000; i++);
  out_LCD_low( Ch );
}

void lcd_task(void *pvParameters)
/*****************************************************************************
*   Input    : pvParameters (unused)
*   Output   : -
*   Function : FreeRTOS LCD task - initializes and runs the LCD display
******************************************************************************/
{
  INT8U init_idx = 0;
  INT8U *pStr = (INT8U *)pvParameters;
  (void) *pvParameters;
  char msg[] = "LCD Ready";

  /* Perform LCD init sequence using vTaskDelay (scheduler must be running) */
  for (init_idx = 0; LCD_init_sequense[init_idx] != 0xFF; init_idx++)
  {
    wr_ctrl_LCD(LCD_init_sequense[init_idx]);
    vTaskDelay(pdMS_TO_TICKS(5));
    vTaskDelay(pdMS_TO_TICKS(5));
  }
  vTaskDelay(pdMS_TO_TICKS(5));

  /* Keep task alive */
  while (1)
  {
   if (xQueueReceive(xLCDQueue, &pStr, portMAX_DELAY) == pdTRUE)
   {
       clr_LCD();
       // Receive and print to LCD
       wr_str_LCD(pStr);
   }
  }
}


/****************************** End Of Module *******************************/




