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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 2
#define TOOGLE 3
#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/
struct leds
{
	uint8_t mode;
	uint8_t n_led;
	uint8_t n_ciclos;
	uint16_t periodo;
} my_leds;
/*==================[internal functions declaration]=========================*/
void funcionLed(struct leds *led)

{
	switch (led->mode)
	{
	case ON:
		switch (led->n_led)
		{
		case 1:
			LedOn(LED_1);
			break;
		case 2:
			LedOn(LED_2);
			break;
		case 3:
			LedOn(LED_3);
		default:
			break;
		}
	case OFF:
		switch (led->n_led)
		{
		case 1:
			LedOff(LED_1);
			break;
		case 2:
			LedOff(LED_2);
			break;
		case 3:
			LedOff(LED_3);
		default:
			break;
		}
	case TOOGLE:
		for (int i = 0; i < led->n_ciclos; i++)
		{
			switch (led->n_led)
			{
			case 1:
				LedToggle(LED_1);
				break;
			case 2:
				LedToggle(LED_2);
				break;
			case 3:
				LedToggle(LED_3);
			default:
				break;
			}
			for (int j = 0; j < led->periodo/CONFIG_BLINK_PERIOD; j++)
			{

				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
			}
		}
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");
	LedsInit();
	SwitchesInit();
	my_leds.mode = TOOGLE;
	my_leds.n_ciclos = 10;
	my_leds.periodo = 500;
	my_leds.n_led = 1;
	funcionLed(&my_leds);
}
/*==================[end of file]============================================*/
