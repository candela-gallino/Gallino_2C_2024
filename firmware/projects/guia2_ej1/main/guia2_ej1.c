/*! @mainpage Ejercicio 1 proyecto 2 
 *
 * @section genDesc General Description
 *
 * Diseñar el firmware modelando con un diagrama de flujo de manera que cumpla con las 
 * siguientes funcionalidades:
 * Mostrar distancia medida utilizando los leds de la siguiente manera:
 * Si la distancia es menor a 10 cm, apagar todos los LEDs.
 * Si la distancia está entre 10 y 20 cm, encender el LED_1.
 * Si la distancia está entre 20 y 30 cm, encender el LED_2 y LED_1.
 * Si la distancia es mayor a 30 cm, encender el LED_3, LED_2 y LED_1.
 * Mostrar el valor de distancia en cm utilizando el display LCD.
 * Usar TEC1 para activar y detener la medición.
 * Usar TEC2 para mantener el resultado (“HOLD”).
 * Refresco de medición: 1 s

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
/** 
 * @brief Manejador de la tarea de teclas.
 */
TaskHandle_t teclas_task_handle = NULL;
/** 
 * @brief Manejador de la tarea de medición.
 */
TaskHandle_t medir_task_handle = NULL;
/** 
 * @brief Manejador de la tarea de visualización.
 */
TaskHandle_t mostrar_task_handle = NULL;
/** 
 * @brief Manejador de la tarea de LEDs.
 */
TaskHandle_t leds_task_handle = NULL;
/** 
 * @brief Indica si el sistema está encendido.
 */
bool on = true;

bool hold = false;
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Tarea para manejar las entradas de las teclas.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
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
/**
 * @brief Tarea para medir la distancia utilizando un sensor 
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
static void medirTask(void *pvParameter)
{

	while (true)
	{
		if (on)
			distancia = HcSr04ReadDistanceInCentimeters();
		vTaskDelay(TIEMPO_DELAY / portTICK_PERIOD_MS);
	}
}
/**
 * @brief Tarea para mostrar la distancia medida 
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
		vTaskDelay(TIEMPO_DELAY / portTICK_PERIOD_MS);
	}
}
/**
 * @brief Tarea para controlar el estado de los LEDs en función de la distancia medida.
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