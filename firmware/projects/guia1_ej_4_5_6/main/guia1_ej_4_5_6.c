/*! @mainpage Ejercicio 4-5-6 Proyecto 1
 *
 * @section genDesc General Description
 * El proyecto consiste en recibir un dato de 32 bits el cual se va a mostrar en el Display 
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    PERIFERICO  |   EDU-ESP   	|
 * |:--------------:|:--------------|
 * | 	D1   	 	| 	GPIO_20
 * |:--------------:|:--------------|
 * | 	D2    	 	| 	GPIO_21
 * |:--------------:|:--------------|
 * | 	D3   	 	| 	GPIO_22
 * |:--------------:|:--------------|
 * | 	D4   	 	| 	GPIO_23
 * |:--------------:|:--------------|
 * | 	SEL_1   	| 	GPIO_19
 * |:--------------:|:--------------|
 * | 	SEL_2   	| 	GPIO_18
 * |:--------------:|:--------------|
 * | 	SEL_3   	| 	GPIO_9
 * |:--------------:|:--------------|
 * | 	5V   	 	| 	5V
 * |:--------------:|:--------------|
 * | 	GND   	 	| 	GND         | 
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 29/08/2024 | Document creation		                         |
 *
 * @author Gallino Candela (candela.gallino@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
//estructura para generar gpio
typedef struct
{
	gpio_t pin;
	io_t dir;
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
/** @fn convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
 * @brief  		Convierte el dato a BCD 
 * @param[in]  	data: dato de 32 bits
 * @param[in]  	digits: dato en BCD de 8 bits 
 * @param[out]  bcd_number: arreglo que guarda los datos 
 * @retval	cero
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{

	for (int i = 0; i < digits; i++)
	{

		bcd_number[i] = data % 10;
		data = data / 10;
	}

	return 0;
}
// esta funcion lo que va a hacer es recibir el digito del arreglo (por ejemplo va a recibirme si es GPIO20,GPIO21 etc..) y va a recorrer un for en donde
// por ejemplo si tenemos 0110&0001 tendriamos un cero (ver lo que hace &) estaria en bajo por lo que llamamos al GPIOOff
/**
 * @fn Mapear bits
 * @brief  		Esta funcion va a recibir un digito en BCD y va a mapear los bits bo-> GPIO_20, b1-> GPIO_21
 *              b2->GPIO_22 b2->GPIO_23
 * @param[in]  	arreglo: vector de estructuras del tipo gpioConf_t
 * @param[in]  	digits: digito BCD
 * @retval 		None
 */
void mapearbits(gpioConf_t *arreglo, uint8_t digits)
{
	for (int j = 0; j < 4; j++)
	{
		if (digits & 1 << j)
			GPIOOn(arreglo[j].pin);
		else
			GPIOOff(arreglo[j].pin);
	}
}
/**
 * @fn Mostrar tres digitos 
 * @brief  		Esta funcion va a recibir un digito de 32 bits y va a mapear los digitos con los puertos del LCD
 *              siendo digito1->GPIO_19 digito2->GPIO_18 digito3->GPIO_9 
 * @param[in]  	arreglo1: arreglo del tipo gpioConf_t
 * @param[in]  	arreglo2: arreglo del tipo gpioConf_t
 * @param[in]  	digitoin: dato de 32 bits
 * @param[in]  	digitoout: digito de salida de 8 bits
 * @retval 		None
 */

void mostrar_tres_digitos(gpioConf_t *arreglo1, gpioConf_t *arreglo2, uint32_t digitoin, uint8_t digitoout)
{
	uint8_t bcd_number[digitoout];
	convertToBcdArray(digitoin, digitoout, bcd_number);

	for (int j = 0; j < digitoout; j++)
	{

		mapearbits(arreglo1, bcd_number[j]);
		GPIOOn(arreglo2[j].pin);

		GPIOOff(arreglo2[j].pin);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");

	uint32_t data = 245;
	uint8_t digits = 3;
	uint8_t bcd_number[digits];

	convertToBcdArray(data, digits, bcd_number);
	for (int i = 0; i < digits; i++)
	{

		printf("El valor de cada digito es %d\n", bcd_number[i]);
	}

	gpioConf_t arreglo[] =
		{
			{GPIO_20, GPIO_OUTPUT},
			{GPIO_21, GPIO_OUTPUT},
			{GPIO_22, GPIO_OUTPUT},
			{GPIO_23, GPIO_OUTPUT},

		};
	for (int i = 0; i < 4; i++)
	{

		GPIOInit(arreglo[i].pin, arreglo[i].dir);
	}
	//uint8_t digito = 6;
	//mapearbits(arreglo, digito);

gpioConf_t arreglo2[] =
	{

		{GPIO_9, GPIO_OUTPUT},
		{GPIO_18, GPIO_OUTPUT},
		{GPIO_19, GPIO_OUTPUT}
	};
for (int i = 0; i<digits; i++)
{

	GPIOInit(arreglo2[i].pin, arreglo2[i].dir);
}

uint32_t digito = 123;
mostrar_tres_digitos(arreglo,arreglo2,digito,digits);
}

	/*==================[end of file]============================================*/