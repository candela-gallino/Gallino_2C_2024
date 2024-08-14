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
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			
	io_t dir;			
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{

	for(int i=0;i<digits;i++){

     bcd_number[i]=data%10;
	 data=data/10;

	}

return 0;
} 
//esta funcion lo que va a hacer es recibir el digito del arreglo (por ejemplo va a recibirme si es GPIO20,GPIO21 etc..) y va a recorrer un for en donde 
//por ejemplo si tenemos 0110&0001 tendriamos un cero (ver lo que hace &) estaria en bajo por lo que llamamos al GPIOOff
void mapearbits (gpioConf_t *arreglo, uint8_t digits){
for(int j=0;j<4;j++){
	if(digits&1<<j) 
	GPIOOn(arreglo[j].pin);
	else
	GPIOOff(arreglo[j].pin);
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

	gpioConf_t arreglo[]=
{
	{GPIO_20,GPIO_OUTPUT},
	{GPIO_21,GPIO_OUTPUT},
	{GPIO_22,GPIO_OUTPUT},
	{GPIO_23,GPIO_OUTPUT},

};
	for (int i = 0; i < 4; i++)
	{

		GPIOInit(arreglo[i].pin, arreglo[i].dir);
	}
	uint8_t digito = 6;
	mapearbits(arreglo, digito);
}
/*==================[end of file]============================================*/