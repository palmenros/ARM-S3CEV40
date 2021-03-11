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
#include "uart.h"
#include "rtc.h"
#include "common_types.h"
#include "at24c04.h"
#include "lcd.h"
#include "iic.h"
#include "tests/test_sound_effects.h"
#include "tests/test_WAV.h"
#include "ts.h"
#include "multitasking/foreback.h"
#include "multitasking/queue.h"
#include "multitasking/cyclic.h"
#include "multitasking/non_expropiative.h"
#include "kernel/kernel_entry.h"

// BMP Images sourced by using GDB
#define ARBOL      ((uint8 *)0x0c210000)
#define PADRINO    ((uint8 *)0x0c220000)
#define PICACHU    ((uint8 *)0x0c230000)
#define HARRY      ((uint8 *)0x0c240000)
#define CHAPLIN    ((uint8 *)0x0c250000)
#define PULP       ((uint8 *)0x0c260000)
#define MAPA       ((uint8 *)0x0c270000)

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));
void rtc_ISR(void)  __attribute__ ((interrupt ("IRQ")));
void rtc_alarm_ISR(void)  __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void)
{
	//Timer ISR Implementation
	led1_switch();
	led2_switch();

	ic_cleanflag(INT_TIMER0);
}

void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	if (buttons & BUT1) {
		//BUT1 pressed
	}

	if (buttons & BUT2) {
		//BUT2 pressed
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
	static unsigned n = 0;
	int key = -1;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	n = (n+1) & 0xf;

	if (key != -1) {
		/* Visualizacion en el display */
		//COMPLETAR: mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h

		D8Led_digit(key);
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
		//while (/*COMPLETAR: true si está pulsada la tecla (leer del registro rPDATG)*/);
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */
	//COMPLETAR
	//borrar la interrupción por la línea EINT1 en el registro rI_ISPC
    ic_cleanflag(INT_EINT1);
}

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(0x02); // "-" Symbol
	led1_on();

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

	//TIMER0: 0.5 seconds
	//tmr_set_mode(TIMER0, RELOAD);
	//tmr_set_prescaler(TIMER0, 255);
	//tmr_set_divider(TIMER0, D1_8);
	//tmr_set_count(TIMER0, 62500 >> 3, 1);

	//tmr_update(TIMER0);

	//COMPLETAR: tomar el código de la segunda parte

	//tmr_start(TIMER0);

	//if (RL.moving)
	//
	/***************************/

	// Registramos las ISRs
	//pISR_TIMER0 = (unsigned)timer_ISR;//COMPLETAR: registrar la RTI del timer
	//pISR_EINT4567 = (unsigned)button_ISR; //COMPLETAR: registrar la RTI de los botones
	//pISR_EINT1    = (unsigned)keyboard_ISR; //COMPLETAR: registrar la RTI del teclado

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado 
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	uart0_init();
	iic_init();
	rtc_init();
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
	ic_conf_line(INT_EINT1, IRQ);

	//ic_enable(INT_TIMER0);
	//ic_enable(INT_EINT4567);
	//ic_enable(INT_EINT1);

	ic_enable(INT_GLOBAL);

	lcd_init();
	//ts_init();
	//ts_on();

	Delay(0);
	return 0;
}

void read_date(rtc_time_t* rtc_time)
{
	uart0_puts( "\nDia: " );
	rtc_time->mday = uart0_getint();
	uart0_puts( "\nMes: " );
	rtc_time->mon = uart0_getint();
	uart0_puts( "\nA�o: " );
	rtc_time->year = uart0_getint();
	uart0_puts( "\nDia de la semana: " );
	rtc_time->wday = uart0_getint();

	uart0_puts( "\nHora: " );
	rtc_time->hour = uart0_getint();
	uart0_puts( "\nMinutos: " );
	rtc_time->min = uart0_getint();
	uart0_puts( "\nSegundos: " );
	rtc_time->sec = uart0_getint();
}

void rtc_ISR() {
	static rtc_time_t rtc_time;

	rtc_gettime(&rtc_time);
	uart0_put_rtc_time(&rtc_time);
	uart0_putchar('\n');

	ic_cleanflag(INT_TICK);
}

void rtc_alarm_ISR(void) {
	uart0_puts("\nALARM GOES OFF!\n");
	ic_cleanflag(INT_RTC);
}

void clock_setup() {
	static rtc_time_t rtc_time;
	uart0_puts("Initial time: \n");
	rtc_gettime(&rtc_time);
	uart0_put_rtc_time(&rtc_time);

	//Ask for time first time
	read_date(&rtc_time);
	rtc_puttime(&rtc_time);

	//Set alarm 10 seconds from now (imperfect but enough)

	rtc_time.sec += 10;
	if(rtc_time.sec >= 60) {
		rtc_time.sec -= 60;
		rtc_time.min++;
	}

	if(rtc_time.min >= 60) {
		rtc_time.min -= 60;
		rtc_time.hour++;
	}

	if(rtc_time.hour >= 24) {
		rtc_time.hour--;
		rtc_time.mday++;
	}

	uart0_puts("Alarm time: ");
	uart0_put_rtc_time(&rtc_time);
	uart0_putchar('\n');

	rtc_set_alarm(rtc_alarm_ISR, &rtc_time);
}

void startup() {
	kernel_entry();
}

int loop(void) {
	return 0;
}

int main(void) {

	setup();
	startup();

	while (1) {
		loop();
	}
}
