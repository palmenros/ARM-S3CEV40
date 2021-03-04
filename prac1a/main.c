#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "gpio.h"

struct RLstat {
	int moving;
	int speed;
	int iter;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.iter = 0,
	.direction = 0,
	.position = 0,
};

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para espera activa */

	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h hay
	//que configurar los pines 6 y 7 del puerto G como pines de entrada y
	//activar las correspondientes resistencias de pull-up
	
	portG_conf(6, INPUT);
	portG_conf_pup(6, ENABLE);

	portG_conf(7, INPUT);
	portG_conf_pup(7, ENABLE);

	/*******************************************/

	Delay(0);
	return 0;
}

int loop(void)
{
	unsigned int buttons = read_button();
	unsigned int contador_pulsaciones = 0;

	if (buttons & BUT1) {
		// COMPLETAR: utilizando la interfaz para los leds definida en leds.h
		// hay que apagar ambos leds
		led1_off();
		led2_off();

		// También hay que conmutar la dirección del movimiento del led rotante
		// representado por el campo direction de la variable RL
		RL.direction = !RL.direction;
	}

	if (buttons & BUT2) {
		// COMPLETAR: utilizando la interfaz para los leds definida en leds.h
		// Incrementar contador de pulsaciones. Si es par, conmUtar led1. Si es impar, conmutar el led2.

		contador_pulsaciones++;

		if(contador_pulsaciones & 1)
			//impar
			led2_switch();
		else
			//par
			led1_switch();

		// También hay que conmutar el estado de movimiento del led rotante
		// representado por el campo moving de la variable RL, y en caso de
		// ponerlo en marcha debemos reiniciar el campo iter al valor del campo
		// speed.

		RL.moving = !RL.moving;
		if(RL.moving)
			RL.iter = RL.speed;
	}

	if (RL.moving) {
		RL.iter--;
		if (RL.iter == 0) {
			// COMPLETAR: debemos hacer avanzar el led rotante una posición en
			// la dirección indicada por el campo direction de la variable RL.


			// La posición actual está representada en el campo position.
			// Recordar que queremos un movimiento circular, representado por
			// las 6 primeras posiciones en el array Segmentes del display de 8
			// segmentos, por lo que position debe estar siempre entre 0 y 5.
			
			//	   0
			//    __
			// 5 |  | 1
			//   |__|
			//   |  | 2
			// 4 |__|
			//     3

			if(RL.direction) {
				//Horario
				RL.position++;

				if(RL.position == 6)
					RL.position = 0;

			} else {
				//Antihorario
				RL.position--;

				if(RL.position == -1)
					RL.position = 5;
			}

			D8Led_segment(RL.position);
			RL.iter = RL.speed;
		}
	}

	Delay(2000); // espera de 200ms para que el bucle se repita 5 veces por segundo
	return 0;
}


int main(void)
{
	setup();

	while (1) {
		loop();
	}
}
