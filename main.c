#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"
#include "key.h"
#include "gpio.h"
#include "queue.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define MED_PRIO  2
#define LOW_PRIO  1
#define QUEUE_LEN 16

// Global queue
QueueHandle_t xLCDQueue;

// Simple task to bridge keyboard and LCD
void keyboard_to_lcd_task(void *pvParameters)
{
    (void)pvParameters;
    INT8U key_val;
    INT8U buffer[2] = {0, 0}; // Buffer to hold the key character and null terminator
    QueueHandle_t keyQueue = key_queue_handle(); // Get handle

    while(1)
    {
        // Wait for a key press
        if(xQueueReceive(keyQueue, &key_val, portMAX_DELAY) == pdTRUE)
        {
            buffer[0] = key_val;
            // Send the pointer to the buffer to the LCD queue
            INT8U *pBuffer = buffer;
            xQueueSend(xLCDQueue, &pBuffer, portMAX_DELAY);
        }
    }
}

static void setupHardware(void){
  init_systick();
  init_gpio();
  key_init(); // Initialize keyboard
}

int main(void)
{
    setupHardware();
    
    // Initialize LCD queue
    xLCDQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));

    // Create tasks
    xTaskCreate(lcd_task, "lcd", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    key_create_task(LOW_PRIO); // Create keypad task
    xTaskCreate(keyboard_to_lcd_task, "bridge", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);

    vTaskStartScheduler();

    while(1)
    {
        // Should not reach here
    }
}
