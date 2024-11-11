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
 * | apertura_cierre| 	GPIO_0      |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/11/2024 | Document creation		                         |
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
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
bool distancia_10=false;
uint16_t distancia_inicial;
uint16_t velocidad;
uint16_t distancia_final=0;
uint16_t tiempo,tiempo_final,tiempo_inicial;
uint16_t pesototal=0;
uint16_t velocidad_maxima;
bool abrir_barrera=false;
bool cerrar_barrera=false;
TaskFunction_t medir_distancia_taskhandle=NULL;
TaskHandle_t medir_velocidad_taskhandle=NULL;
TaskHandle_t prender_leds_tasnkandle=NULL;
TaskFunction_t mostrar_x_pantalla_taskhandle=NULL;
TaskHandle_t peso_taskhandle=NULL;
/*==================[internal functions declaration]=========================*/
/** @fn medir_distancia
 * @brief  		mide la distancia de la camioneta 
 * @retval	cero
 */
void medir_distancia (void *param){
	uint16_t i=0;
	while (true)
	
	{ if(i<10)
		
		distancia_inicial=HcSr04ReadDistanceInCentimeters();
		
	}
	vTaskDelay(1000/portTICK_PERIOD_MS);
	distancia_final=distancia_final-distancia_inicial;

	tiempo=i*1000;

    /** @fn medir_velocidad
 * @brief  		mide la velocidad con la que viene el camion y tambien la velocidad maxima
 * @retval	cero
 */
}

void medir_velocidad (void *param){
	float maxima_velocidad_aux=0;
	while (true)
	{
		if(distancia_final<1000){
     velocidad=(distancia_final*1000)/tiempo;
		if(velocidad>maxima_velocidad_aux){
		maxima_velocidad_aux=velocidad;
		}
		else{
			velocidad_maxima=maxima_velocidad_aux;
			maxima_velocidad_aux=0;
		}
	}
	vTaskDelay(100/portTICK_PERIOD_MS);
	
}}

/** @fn prender_leds
 * @brief  	prende leds de distinto color dependiendo la velocidad 
 * @retval	cero
 */
void prender_leds (void *param){
	while (true)
	{
	{ if(velocidad>8){

		LedOn(LED_3);
	}
	else 
	   LedOff(LED_3);
	if(velocidad>0 && velocidad<8){
		LedOn(LED_2);
	}
	else{
	LedOff(LED_2);
	}
	if(velocidad==0){
	LedOn(LED_1);

	}
	else{
		LedOff(LED_1);
	}
	
	}
	 vTaskDelay(1000 / portTICK_PERIOD_MS);

}}
/** @fn peso
 * @brief  	determina el peso del camion  
 * @retval	cero
 */
void peso (void *param){
	uint16_t galga1,galga2;
	uint16_t pesogalga1,pesogalga2,pesogalgaaux1,pesogalgaaux2;
	int i=0;
	
	while (true)
	{
		if(velocidad==0){
		if(i<50){
		AnalogInputReadSingle(CH1, &galga1);
		pesogalga1+=(galga1*20000)/3300;
		
		}
		if(i<50){
		AnalogInputReadSingle(CH2, &galga2);
	    pesogalga2+=(galga2*20000)/3300;
	
		}
		
		pesototal=(pesogalga1/50)+(pesogalga2/50);
		}
	
	vTaskDelay(5/portTICK_PERIOD_MS);
	}
}

/** @fn mostrar_x_pantalla
 * @brief  	muestra por pantalla la velocidad maxima y el peso 
 * @retval	cero
 */

void mostrar_x_pantalla(void* param){
	while (true)
	{
	           
			    UartSendString(UART_PC, " Peso: ");
				UartSendString(UART_PC, (char *)UartItoa(pesototal, 10));
				UartSendString(UART_PC, " kg\r\n");

				 UartSendString(UART_PC, " Velocidad maxima: ");
				UartSendString(UART_PC, (char *)UartItoa(velocidad_maxima), 10));
				UartSendString(UART_PC, " m/s");

	
	vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

/** @fn apertura_cierra
 * @brief  	permite abrir o cerrar de una barrera mandando por consola "o" o "c"
 * @retval	cero
 */
void apertura_cierre (void *param){
	uint8_t teclas;
	UartReadByte(UART_PC, &teclas);

	if (teclas == 'o')
	{
	 abrir_barrera=true;
	 GPIOOn(GPIO_0);
	}
	else
	GPIOOff(GPIO_0);
	 if (teclas == 'c')
	{
		cerrar_barrera=true;
		GPIOOn(GPIO_0);
	}
	else
	GPIOOff(GPIO_0);
}


void 
/*==================[external functions definition]==========================*/
void app_main(void){
	printf("Hello world!\n");

	LedsInit();
	GPIOInit(GPIO_0,GPIO_INPUT);
	HcSr04Deinit(GPIO_3,GPIO_2);

analog_input_config_t config1 =
			{
				.input = CH1,
				.mode = ADC_SINGLE,
				.func_p = NULL,
				.param_p = NULL,
				.sample_frec = 0};
		AnalogInputInit(&config1);

		analog_input_config_t config2 =
			{
				.input = CH2,
				.mode = ADC_SINGLE,
				.func_p = NULL,
				.param_p = NULL,
				.sample_frec = 0};
		AnalogInputInit(&config2);

		serial_config_t config = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = mostrar_x_pantalla,
		.param_p = NULL};

	UartInit(&config);

	serial_config_t config3 = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = apertura_cierre,
		.param_p = NULL};
		UartInit(&config3);

    xTaskCreate(&medir_distancia,"MEDIR_DIS", 512, NULL, 5, &medir_distancia_taskhandle);
	xTaskCreate(&medir_velocidad, "MEDIR_VEL", 512, NULL, 5, &medir_velocidad_taskhandle);
	xTaskCreate(&peso, "PESO", 512, NULL, 5, &peso_taskhandle);
	xTaskCreate(&mostrar_x_pantalla, "MOSTRAR_PC", 512, &config, 5, &mostrar_x_pantalla_taskhandle);
	xTaskCreate(&prender_leds, "PRENDER", 512, NULL, 5, &prender_leds_tasnkandle);

}
/*==================[end of file]============================================*/