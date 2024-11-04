/*! @mainpage PARCIAL 
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
 * | 04/11/2024 | Document creation		                         |
 *
 * @author Gallino Candela  (candela.gallino@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_ALTO
#define GPIO_BAJO
/*==================[internal data definition]===============================*/
uint16_t distancia;
TaskHandle_t medir_distancia_task_handle = NULL;
TaskHandle_t activar_alarma_task_handle = NULL;
TaskHandle_t acelerometro_task_handle = NULL;
uint16_t guardado;

/*==================[internal functions declaration]=========================*/
/** @fn medir_distancia
 * @brief  		mide la distancia entre la bicicleta y vehiculos encendiendo leds dependiendo la distancia medida 
 * @retval	cero
 */
void medir_distancia (void *param){

	while (true)
	{
		distancia=HcSr04ReadDistanceInCentimeters();
	}
	 if(distancia>5)
		LedOn(LED_1);
	else 
	if(distancia>5 && distancia<3){
		LedOn(LED_2);
		LedOn(LED_1)
	}
	else
	if(distancia <3)
	LedOn(LED_2);
	LedOn(LED_3);
	 vTaskDelay(2000 / portTICK_PERIOD_MS);
}

/** @fn activar_alarma
 * @brief  		hace sonar el buzzer cuando gpio esta en alto y da mensajes en caso de peligro o precaucion
 * @retval	cero
 */
void activar_alarma (void *param){
	while (true)
	{
	
	if (distancia<3)
	{
		GPIOOn(GPIO_0);
		sonar_buzzer();
		UartSendString(UART_CONNECTOR, (char *)UartItoa(distancia, 10));
		UartSendString(UART_CONNECTOR, "Peligro,vehiculo cerca");
			
	}
	vTaskDelay(1000/portTICK_PERIOD_MS);
	if(distancia>5 && distancia<3)
	{
		GPIOOff(GPIO_0);
		sonar_buzzer();
		UartSendString(UART_CONNECTOR, (char *)UartItoa(distancia, 10));
		UartSendString(UART_CONNECTOR, " Precaucion,vehiculo cerca");
	
	}
	vTaskDelay(500/portTICK_PERIOD_MS)
}

/** @fn funcion_acelerometro
 * @brief  		detecta golpes y caidas y manda mensaje en caso de caida 
 * @retval	cero
 */
void funcion_con_acelerometro (void *param){
	uint16_t dato1,dato2,dato3; 
	float suma;
	acelerometro(eje1,eje2,eje3);
	AnalogInputReadSingle(CH1, &eje1);
	AnalogInputReadSingle(CH1, &eje2);
	AnalogInputReadSingle(CH1, &eje3);

	dato1=(eje1/3.5)*5.5;
	dato2=(eje2/3.5)*5.5;
	dato3=(eje3/3.5)*5.5
	
	suma=dato1+dato2+dato3;

	if(suma>4){
		UartSendString(UART_CONNECTOR, (char *)UartItoa(suma, 10));
		UartSendString(UART_CONNECTOR, "caida detectada ");
	}

}

/*==================[external functions definition]==========================*/
void app_main(void){
	printf("Hello world!\n");
	GPIOInit(GPIO_0, GPIO_OUTPUT);
	HcSr04Deinit(GPIO_3,GPIO_2);

	xTaskCreate(&medir_distancia "MEDIR", 512, NULL, 5, &medir_distancia_task_handle);
	xTaskCreate(&activar_alarma, "ACTIVAR", 512, NULL, 5, &activar_alarma_task_handle);
	xTaskCreate(&funcion_con_acelerometro, "CAIDA", 512, NULL, 5, &acelerometro_task_handle);

analog_input_config_t config =
			{
				.input = CH1,
				.mode = ADC_SINGLE,
				.func_p = NULL,
				.param_p = NULL,
				.sample_frec = 0};
		AnalogInputInit(&config);
}
/*==================[end of file]============================================*/