
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

//definições para os pinos dos LEDs
#define LED1 GPIO_NUM_2 
#define LED2 GPIO_NUM_4  


void blink_task(void *pvParameter); //Protótipo da função que será usada como a tarefa FreeRTOS

void app_main(void)
{
    gpio_pad_select_gpio(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT); //saída digital

    gpio_pad_select_gpio(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT); //saída digital

    /* Criar a tarefa de piscar LEDs */
    xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
}

void blink_task(void *pvParameter)
{
    while(1) {
        //Liga o LED1 e desliga o LED2
        gpio_set_level(LED1, 1);
        gpio_set_level(LED2, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS); //Espera 500 ms

        //Desliga o LED1 e liga o LED2
        gpio_set_level(LED1, 0);
        gpio_set_level(LED2, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS); //Espera 500 ms
    }
}

