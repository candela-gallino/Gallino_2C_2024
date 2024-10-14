/*! @mainpage Ejercicio 3 proyecto 2
 *
 * @section genDesc General Description
 *
 *Cree un nuevo proyecto en el que modifique la actividad del punto 2 agregando ahora el 
 *puerto serie. Envíe los datos de las mediciones para poder observarlos en un terminal en la PC, 
 *siguiendo el siguiente formato:
 *3 dígitos ascii + 1 carácter espacio + dos caracteres para la unidad (cm) + cambio de línea “ \r\n”
 *Además debe ser posible controlar la EDU-ESP de la siguiente manera:
 *Con las teclas “O” y “H”, replicar la funcionalidad de las teclas 1 y 2 de la EDU-ESP

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
#include "uart_mcu.h"
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
 * @brief Tarea para medir la distancia usando el sensor HC-SR04.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
static void medirTask(void *pvParameter)
{
	while (true)
	{
		if (on)
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}
/**
 * @brief Tarea para mostrar la distancia medida en el LCD y enviar por UART.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado).
 */
static void mostrarTask(void *pvParameter)
{
	while (true)
	{
		if (on)
		{
			UartSendString(UART_PC, (char *)UartItoa(distancia, 10));
			UartSendString(UART_PC, " cm\n\r");
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
 * @brief Revisa las teclas presionadas y cambia el estado de las banderas.
 */
void revisarTeclas()
{
	uint8_t teclas;
	UartReadByte(UART_PC, &teclas);

	if (teclas == 'O')
	{
		on = !on;
	}
	else if (teclas == 'H')
	{
		hold = !hold;
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
		// vTaskDelay(PERIODO_TIMER_B / portTICK_PERIOD_MS);
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
	LedsInit();
	SwitchesInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);

	xTaskCreate(&medirTask, "MEDIR", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&mostrarTask, "MOSTRAR",2048, NULL, 5, &mostrar_task_handle);
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

	serial_config_t serial_global = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = revisarTeclas,
		.param_p = NULL};

	UartInit(&serial_global);
}
/*==================[end of file]============================================*/