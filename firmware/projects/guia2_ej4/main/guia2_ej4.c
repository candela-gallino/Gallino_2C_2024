/*! @mainpage Ejercicio 4 del proyecto 2 
 *
 * @section genDesc General Description
 *Diseñar e implementar una aplicación, basada en el driver analog_io_mcu.h y el driver de 
 *transmisión serie uart_mcu.h, que digitalice una señal analógica y la transmita a un graficador 
 *de puerto serie de la PC. Se debe tomar la entrada CH1 del conversor AD y la transmisión se debe 
 *realizar por la UART conectada al puerto serie de la PC, en un formato compatible con un 
 *graficador por puerto serie. 

 * This section describes how the program works.
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
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/10/2024| Document creation		                         |
 *
 * @author Gallino Candela (candela.gallino@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"
#include "led.h"
#include "switch.h"

/*==================[macros and definitions]=================================*/
/*! @def BUFFER_SIZE
 *  @brief Tamaño del buffer para almacenar datos del ECG.
 */
#define BUFFER_SIZE 231
/*! @var TaskHandle_t cambio
 *  @brief Manejador de la tarea para la función `Funcion`.
 */
TaskHandle_t cambio = NULL;

/*==================[internal data definition]===============================*/
/*! @var TaskHandle_t main_task_handle
 *  @brief Manejador de la tarea para la función `mostrarecg`.
 */
TaskHandle_t main_task_handle = NULL;
/*! @var const char ecg[BUFFER_SIZE]
 *  @brief Datos de ejemplo de un ECG.
 */
/*const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};*/

unsigned char ecg[BUFFER_SIZE] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17

} ;

/*==================[internal functions declaration]=========================*/
/*! @brief Función de callback para el temporizador A.
 *
 *  Esta función se llama desde una interrupción del temporizador A y notifica
 *  a la tarea `cambio`.
 *
 *  @param param Puntero a parámetros de la función (no utilizado).
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR( cambio, pdFALSE); 
}
/*! @brief Función de callback para el temporizador B.
 *
 *  Esta función se llama desde una interrupción del temporizador B y notifica
 *  a la tarea `main_task_handle`.
 *
 *  @param param Puntero a parámetros de la función (no utilizado).
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR( main_task_handle, pdFALSE); 
}
/*! @brief Función que lee una entrada analógica y envía el valor por UART.
 *
 *  Esta función espera una notificación para leer un valor analógico y enviarlo
 *  a través de UART.
 *
 *  @param param Puntero a parámetros de la función (no utilizado).
 */
void Funcion(void* param)
{
	uint16_t guardado;
	while (true)
	{ 
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1,&guardado);
		UartSendString(UART_PC, (char*)UartItoa(guardado, 10));
		UartSendString(UART_PC, "\r"); 
	}
}
/*! @brief Función que muestra los datos del ECG.
 *
 *  Esta función espera una notificación para escribir un valor del ECG en la
 *  salida analógica.
 */
void mostrarecg()
{
	int i=0;
while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(i<231)
			{AnalogOutputWrite(ecg[i]);}
		else
			{i=0;}
		i++;
	}
}
/*==================[external functions definition]==========================*/

/*! @brief Función principal del programa.
 *
 *  Configura las entradas analógicas, temporizadores y UART, y crea las tareas
 *  necesarias para el funcionamiento del programa.
 */
void app_main(void){
	
	analog_input_config_t config =
	{
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
	};
	AnalogInputInit(&config);
	
	timer_config_t timer_led_1 = 
	{
		.timer = TIMER_A,
		.period = 2000,
		.func_p = FuncTimerA,
		.param_p = NULL,
	};
	TimerInit(&timer_led_1);

		timer_config_t timer_led_2 = 
	{
		.timer = TIMER_B,
		.period = 4000,
		.func_p = FuncTimerB,
		.param_p = NULL,
	};
	TimerInit(&timer_led_2);

	serial_config_t pantalla = 
	{
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL,
	};
	UartInit(&pantalla);

xTaskCreate(&Funcion, "OSCILOSCOPIO", 2048, NULL, 5, &cambio);
xTaskCreate(&mostrarecg,"ecg",2048,NULL,5,&main_task_handle);
TimerStart(timer_led_1.timer);
TimerStart(timer_led_2.timer);

AnalogOutputInit();
}
/*==================[end of file]============================================*/