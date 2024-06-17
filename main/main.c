#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/gpio.h>

bool is_LED_on = false;
SemaphoreHandle_t mutex = NULL;

void setup()
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    mutex = xSemaphoreCreateMutex();
}

void cpu0_loop()
{
    while(true)
    {
        if(xSemaphoreTake(mutex, portMAX_DELAY))
        {
            gpio_set_level(GPIO_NUM_2, 1);
            is_LED_on = true;
            xSemaphoreGive(mutex);
        }
        vTaskDelay(100);
        if(xSemaphoreTake(mutex, portMAX_DELAY))
        {
            gpio_set_level(GPIO_NUM_2, 0);
            is_LED_on = false;
            xSemaphoreGive(mutex);
        }
        vTaskDelay(100);
    }
}

void cpu1_loop()
{
    while(true)
    {
        if(xSemaphoreTake(mutex, (100 * portTICK_PERIOD_MS)))
        {
            if(is_LED_on == true)
            {
                printf("LED ON\n");
                printf("TICKS: %lu\n", xTaskGetTickCount());
            }
            else
            {
                printf("LED OFF\n");
                printf("TICKS: %lu\n", xTaskGetTickCount());
            }
            xSemaphoreGive(mutex);
            vTaskDelay(100);
        }
    }
}

void app_main(void)
{
    setup();
    xTaskCreatePinnedToCore(cpu0_loop, "blink", 5000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(cpu1_loop, "print", 5000, NULL, 0, NULL, 1);
}
