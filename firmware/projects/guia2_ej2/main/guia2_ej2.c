/*! @mainpage Ejercicio 2 proyecto 2
 *
 * @section genDesc General Description
 *
 * Cree un nuevo proyecto en el que modifique la actividad del punto 1 de manera 
 * de utilizar interrupciones para el control de las teclas y el control de tiempos (Timers).
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V 	 	| 	+5V 		|
 * | 	GND 	 	| 	GND  		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/10/2024 | Document creation		                         |
 *
 * @author Gallino Candela (candela.gallino@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/

#define PERIODO_TIMER_B 1000*1000

/*==================[internal data definition]===============================*/
TaskHandle_t teclas_task_handle = NULL;
TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;
TaskHandle_t leds_task_handle = NULL;
bool on = true;
bool hold = false;
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Tarea para medir la distancia utilizando el sensor HC-SR04.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
static void medirTask(void *pvParameter)
{

	while (true)
	{
		if (on)
			distancia = HcSr04ReadDistanceInCentimeters();
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}
/**
 * @brief Tarea para mostrar la distancia en el LCD.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
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
		// vTaskDelay(PERIODO_TIMER_B / portTICK_PERIOD_MS);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}
/**
 * @brief Tarea para controlar los LEDs según la distancia medida.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
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
		//vTaskDelay(PERIODO_TIMER_B / portTICK_PERIOD_MS);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}
/** @fn read_switch1(void)
 * @brief Función para cambiar el estado de la bandera MedirON al presionar la tecla switch1.
 */
void read_switch1(void)
{
	on = !on;
}

/** @fn read_switch2(void)
 * @brief Función para cambiar el estado de la bandera hold al presionar la tecla switch2.
 */
void read_switch2(void)
{
	hold = !hold;
}

/**
 * @brief Función invocada en la interrupción del timer B
 */


void funcTimerB(void *param)
{
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(leds_task_handle, pdFALSE);
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");
	printf("Hello world!\n");
	LedsInit();
	SwitchesInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);

	xTaskCreate(&medirTask, "MEDIR", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&mostrarTask, "MOSTRAR", 512, NULL, 5, &mostrar_task_handle);
	xTaskCreate(&ledsTask, "LEDS", 512, NULL, 5, &leds_task_handle);

	SwitchActivInt(SWITCH_1, &read_switch1, NULL); // on_off
	SwitchActivInt(SWITCH_2, &read_switch2, NULL); // hold
	
	timer_config_t timer_globalB = {
		.timer = TIMER_B,
		.period = PERIODO_TIMER_B,
		.func_p = funcTimerB,
		.param_p = NULL};
	TimerInit(&timer_globalB);

	
	TimerStart(timer_globalB.timer);
}
/*==================[end of file]============================================*/