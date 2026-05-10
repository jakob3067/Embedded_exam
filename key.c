/**********************************************
* University of Southern Denmark
* Embedded Programing (EMP)
*
* MODULENAME: key.c
* PROJECT: Assignment 7
* DESCRIPTION: See module specification file (.h-file)
* Change log:
***********************************************
* Date of Change
* YYMMDD
* ----------------
* 260410 NK Module created.
*
***********************************************/
/***************** Header *********************/
/***************** Include files **************/
#include "key.h"
#include "FreeRTOS.h"
#include "emp_type.h"
#include "queue.h"
#include "task.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
/***************** Defines ********************/
/***** Pin mapping *****
 * Columns (drive, output): PA2, PA3, PA4
 * Rows    (read,  input):  PE0, PE1, PE2, PE3
 ***********************/
#define COL_MASK 0x1C /* PA2|PA3|PA4 */
#define ROW_MASK 0x0F /* PE0..PE3    */
/***************** Constants ******************/
static QueueHandle_t key_queue;
static const char keymap[3][4] = {
    {'*', '7', '4', '1'}, {'0', '8', '5', '2'}, {'#', '9', '6', '3'}};
static INT8U row_index(INT8U y) {
  switch (y) {
  case 0x01:
    return 1;
  case 0x02:
    return 2;
  case 0x04:
    return 3;
  case 0x08:
    return 4;
  }
  return 0;
}
/***************** Variables ******************/
/***************** Functions ******************/
void key_init(void)
/**********************************************
* Input:
* Output:
* Function: Init gpio pins for keypad.
***********************************************/
{
  /* Enable PORTA and PORTE clocks */
  SYSCTL_RCGCGPIO_R |= (1 << 0) | (1 << 4);
  while ((SYSCTL_PRGPIO_R & ((1 << 0) | (1 << 4))) == 0) {
  }

  /* PA2..PA4 outputs (columns) */
  GPIO_PORTA_DIR_R |= COL_MASK;
  GPIO_PORTA_DEN_R |= COL_MASK;
  GPIO_PORTA_DATA_R &= ~COL_MASK;

  /* PE0..PE3 inputs (rows), pull-down */
  GPIO_PORTE_DIR_R &= ~ROW_MASK;
  GPIO_PORTE_DEN_R |= ROW_MASK;
  GPIO_PORTE_PDR_R |= ROW_MASK;

  key_queue = xQueueCreate(10, sizeof(INT8U));
}

INT8U key_get(void)
/**********************************************
* Input:
* Output: Char from key queue.
* Function: Get char from key queue.
***********************************************/
{
  INT8U ch;
  xQueueReceive(key_queue, &ch, portMAX_DELAY);
  return ch;
}

BOOLEAN get_keyboard(INT8U *pch)
/**********************************************
* Input: Pointer to char.
* Output: TRUE/FALSE (1/0).
* Function: Get char from key queue for file system.
***********************************************/
{
  if (xQueueReceive(key_queue, pch, 0) == pdTRUE)
  {
    return 1;
  }
  return 0;
}

static INT8U scan_column(INT8U col_bit, INT8U col_index)
/**********************************************
* Input: column bit, column index
* Output: Ture/false.
* Function: Scan column for keypress.
***********************************************/
{
  INT8U y;

  GPIO_PORTA_DATA_R &= ~COL_MASK;
  GPIO_PORTA_DATA_R |= col_bit;

  /* tiny settle */
  {
    volatile int d;
    for (d = 0; d < 50; d++) {
    }
  }

  y = GPIO_PORTE_DATA_R & ROW_MASK;
  if (y) {
    INT8U r = row_index(y);
    if (r) {
      INT8U ch = keymap[col_index - 1][r - 1];
      xQueueSend(key_queue, &ch, 0);
      return 1;
    }
  }
  return 0;
}

QueueHandle_t key_queue_handle(void) {
  return key_queue;
}

static void vKeyTask(void *pvParameters)
/**********************************************
* Input:
* Output:
* Function: Keypad task for FreeRTOS.
***********************************************/
{
  TickType_t last = xTaskGetTickCount();
  INT8U pressed;

  for (;;) {
    pressed = 0;
    if (scan_column(0x10, 1))
      pressed = 1; /* PA4 = col 1 */
    else if (scan_column(0x08, 2))
      pressed = 1; /* PA3 = col 2 */
    else if (scan_column(0x04, 3))
      pressed = 1; /* PA2 = col 3 */

    if (pressed) {
      /* wait for release: drive all cols high, watch rows */
      GPIO_PORTA_DATA_R |= COL_MASK;
      while (GPIO_PORTE_DATA_R & ROW_MASK) {
        vTaskDelay(pdMS_TO_TICKS(20));
      }
      GPIO_PORTA_DATA_R &= ~COL_MASK;
    }

    vTaskDelayUntil(&last, pdMS_TO_TICKS(20));
  }
}

void key_create_task(INT8U priority)
/**********************************************
* Input: Task priority
* Output:
* Function: Create keypad task for FreeRTOS
***********************************************/
{
  xTaskCreate(vKeyTask, "key", 128, NULL, priority, NULL);
}
/***************** Enf of module **************/
