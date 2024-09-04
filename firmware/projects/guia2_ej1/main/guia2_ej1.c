/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Candela Gallino (candela.gallino@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"

/*==================[macros and definitions]=================================*/

#define TIEMPO_TECLAS 100
#define TIEMPO_DELAY 1000

/*==================[internal data definition]===============================*/
TaskHandle_t teclas_task_handle = NULL;
TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;
TaskHandle_t leds_task_handle = NULL;
bool on = true;
bool hold = false;
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/

static void teclasTask(void *pvParameter)
{
	uint8_t teclas;

	while (true)
	{
		teclas = SwitchesRead();
		switch (teclas)
		{
		case SWITCH_1:
			on = !on;
			break;
		case SWITCH_2:
			hold = !hold;
			break;
		}
		vTaskDelay(TIEMPO_TECLAS / portTICK_PERIOD_MS);
	}
}

static void medirTask(void *pvParameter)
{

	while (true)
	{
		if (on)
			distancia = HcSr04ReadDistanceInCentimeters();
		vTaskDelay(TIEMPO_DELAY / portTICK_PERIOD_MS);
	}
}

static void mostrarTask(void *pvParameter)
{
	while (true)
	{
		if (on)
		{
			if (!hold) // esto es igual a ==false
			{
				LcdItsE0803Write(distancia);
			}
		}
		else
			LcdItsE0803Off();
		vTaskDelay(TIEMPO_DELAY / portTICK_PERIOD_MS);
	}
}

static void ledsTask(void *pvParameter)
{
	while (true)
	{
		if (on)
		{
			if (distancia < 10)
			{
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}

			else if (distancia >= 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			else if (distancia >= 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else if (distancia >= 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
		}
		else
			LedsOffAll();
		vTaskDelay(TIEMPO_DELAY / portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");
	LedsInit();
	SwitchesInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);

	xTaskCreate(&teclasTask, "TEC_1", 512, NULL, 5, &teclas_task_handle);
	xTaskCreate(&medirTask, "MEDIR", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&mostrarTask, "MOSTRAR", 512, NULL, 5, &mostrar_task_handle);
	xTaskCreate(&ledsTask, "LEDS", 512, NULL, 5, &leds_task_handle);
}
/*==================[end of file]============================================*/