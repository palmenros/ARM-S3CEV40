#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.direction = 0,
	.position = 0,
};

static int contador_pulsaciones = 0;

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void)
{
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

	//Solamente hay que borrar la interrupcion interna
	ic_cleanflag(INT_TIMER0);
}

void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	//COMPLETAR: usar el código de la primera parte parte de atención a los
	//pulsadores

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
		// representado por el campo moving de la variable RL.
		if(RL.moving)
			tmr_stop(TIMER0);
		else
			tmr_start(TIMER0);
		RL.moving = !RL.moving;
	}

	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd
	//COMPLETAR: debemos borrar las peticiones de interrupción en
	//EXTINTPND escribiendo un 1 en los flags que queremos borrar (los
	//correspondientes a los pulsadores pulsados)

	if(buttons & BUT1)
		//Borrar bit 2
		rEXTINTPND |= 1 << 2;

	if(buttons & BUT2)
		//Borrar bit 3
		rEXTINTPND |= 1 << 3;

	ic_cleanflag(INT_EINT4567);
}

void keyboard_ISR(void)
{
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */
		//COMPLETAR: mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h

		switch (key) {
			case 0:
				//COMPLETAR: poner en timer0 divisor 1/8 y contador 62500
				break;
			case 1:
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 31250
				break;
			case 2:
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 15625
				break;
			case 3:
				//COMPLETAR: poner en timer0 timer divisor 1/4 y contador 15625
				break;
			default:
				break;
		}
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
		//while (/*COMPLETAR: true si está pulsada la tecla (leer del registro rPDATG)*/);
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */
	//COMPLETAR
	//borrar la interrupción por la línea EINT1 en el registro rI_ISPC
}

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 1, 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.

	portG_conf(6, EINT);
	portG_eint_trig(6, FALLING);
	portG_conf_pup(6, ENABLE);

	portG_conf(7, EINT);
	portG_eint_trig(7, FALLING);
	portG_conf_pup(7, ENABLE);

	/********************************************************************/

	/* Configuración del timer */

	//TIMER0: 2 segundos

	tmr_set_mode(TIMER0, RELOAD);
	tmr_set_prescaler(TIMER0, 255);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 62500, 1);

	tmr_update(TIMER0);

	//COMPLETAR: tomar el código de la segunda parte

	if (RL.moving)
		tmr_start(TIMER0);
	/***************************/

	// Registramos las ISRs
	pISR_TIMER0 = (unsigned)timer_ISR;//COMPLETAR: registrar la RTI del timer
	pISR_EINT4567 = (unsigned)button_ISR; //COMPLETAR: registrar la RTI de los botones
	//pISR_EINT1    = //COMPLETAR: registrar la RTI del teclado

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado 
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	ic_init();
	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	//		habilitar la línea IRQ en modo vectorizado
	//		deshabilitar la línea FIQ
	//		configurar la línea INT_TIMER0 en modo IRQ
	//		configurar la línea INT_EINT4567 en modo IRQ
	//		configurar la línea INT_EINT1 en modo IRQ
	//		habilitar la línea INT_TIMER0
	//		habilitar la línea INT_EINT4567
	//		habilitar la línea INT_EINT1

	/***************************************************/

	ic_conf_irq(ENABLE, VEC);
	ic_conf_fiq(DISABLE);
	ic_conf_line(INT_TIMER0, IRQ);
	ic_conf_line(INT_EINT4567, IRQ);
	ic_enable(INT_TIMER0);
	ic_enable(INT_EINT4567);
	ic_enable(INT_GLOBAL);

	Delay(0);
	return 0;
}

int loop(void)
{
	return 0;
}


int main(void)
{
	setup();

	while (1) {
		loop();
	}
}
