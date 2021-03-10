/*-------------------------------------------------------------------
**
**  Fichero:
**    lab14.c  14/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Ejemplo de una aplicación guiada por tiempo con arquitectura
**    cyclic executive
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/


#include "../44b.h"
#include "../common_types.h"
#include "../leds.h"
#include "../D8Led.h"
#include "../uart.h"
#include "../button.h"
#include "../keyboard.h"
#include "../intcontroller.h"
#include "../timer.h"
#include "../utils.h"
#include "../rtc.h"
#include "cyclic.h"

#define TICKS_PER_SEC   (100)

#define MAYOR_PERIOD (10000)    /* mcm( 50 ms, 100 ms, 500 ms, 1000 ms, 10000 ms */
#define MINOR_PERIOD (50)       /* 50 ms */

#define NUM_JOBS     (MAYOR_PERIOD/MINOR_PERIOD)

typedef void (*pf_t)(void);

/* Declaración de recursos */

uint8 scancode;
boolean flagTask5;
boolean flagTask6;

volatile boolean flagPb;
volatile boolean flagTimer;

/* Declaración de tareas */

void Task1( void );
void Task2( void );
void Task3( void );
void Task4( void );
void Task5( void );
void Task6( void );
void Task7( void );

/* Declaración de trabajos */

void JobA( void );
void JobB( void );
void JobC( void );
void JobD( void );
void JobE( void );

/* Planificación estática de trabajos */

const pf_t pjobs[NUM_JOBS] =
{
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*   500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  1000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  1500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  2000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  2500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  3000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  3500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  4000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  4500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  5000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  5500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  6000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  6500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  7000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  7500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  8000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  8500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobD,    /*  9000 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobC,    /*  9500 ms */
    JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobB, JobA, JobE     /* 10000 ms */
};

/* Declaración de RTI */

void isr_pb( void ) __attribute__ ((interrupt ("IRQ")));
void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));

/*******************************************************************/

void cyclic( void )
{
    uint8 i;


    /*timers_init();
    uart0_init();
    leds_init();
    segs_init();
    rtc_init();
    pbs_init();
    keypad_init();*/

    uart0_puts( "\n\n Ejecutando una aplicación cyclic executive\n" );
    uart0_puts(     " ------------------------------------------\n\n" ) ;

    flagTask5 = FALSE;    /* Inicializa flags */
    flagTask6 = FALSE;
    flagPb    = FALSE;
    flagTimer = FALSE;

    Task1();    /* Ejecuta por primera vez a las funciones para inicializarlas */
    Task2();
    Task3();
    Task4();
    Task5();
    Task6();
    Task7();

    pISR_EINT4567 = (unsigned) isr_pb;
	ic_enable(INT_EINT4567);

	//MINOR_PERIOD: 50ms
	tmr_set_mode(TIMER0, RELOAD);
	tmr_set_prescaler(TIMER0, 10);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 40000, 1);

	tmr_update(TIMER0);

	pISR_TIMER0 =(unsigned)isr_tick;
	tmr_start(TIMER0);

	ic_enable(INT_TIMER0);

    while( 1 )
    {
        sleep();                        /* Entra en estado IDLE, sale por interrupción */
        if( flagTimer )
        {
        	flagTimer = FALSE;
        	(*pjobs[i])();              /* Las tareas que forman el trabajo se ejecutan en esta hebra (background) */
        	i = ( i==NUM_JOBS-1 ? 0 : i+1 );
        }
    }
}

/*******************************************************************/

void JobA( void )
{
    Task2();
    Task7();
}

void JobB( void )
{
    Task2();
    Task5();
    Task6();
    Task7();
}

void JobC( void )
{
    Task2();
    Task5();
    Task6();
    Task1();
    Task7();
}

void JobD( void )
{
    Task2();
    Task5();
    Task6();
    Task1();
    Task3();
    Task7();
}

void JobE( void )
{
    Task2();
    Task5();
    Task6();
    Task1();
    Task3();
    Task4();
    Task7();
}

/*******************************************************************/

//Task names are used by all 3 multitasking systems. Shut down each other when using others

#if 0
void Task1( void )  /* Cada 0,5 segundos (50 ticks) alterna el led que se enciende */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 1: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
        led1_on();
        led2_off();
    }
    else
    {
        led1_switch();
        led2_switch();
    }
}

void Task2( void )  /* Cada 50 ms (5 ticks) muestrea el keypad y envía el scancode a otras tareas */
{
    static boolean init = TRUE;
    static enum { wait_keydown, scan, wait_keyup } state;

    if( init )
    {
        init  = FALSE;
        uart0_puts( " Task 2: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
        state = wait_keydown;
    }
    else switch( state )
    {
        case wait_keydown:
            if( kb_scan() >= 0 )
                state = scan;
            break;
        case scan:
            scancode = kb_scan();
            if( scancode >= 0 )
            {
                flagTask5 = TRUE;
                flagTask6 = TRUE;
            }
            state = wait_keyup;
            break;
        case wait_keyup:
            if( kb_scan() < 0 )
                state = wait_keydown;
            break;
    }
}

void Task3( void  )  /* Cada segundo (100 ticks) muestra por la UART0 la hora del RTC */
{
    static boolean init = TRUE;
    rtc_time_t rtc_time;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 3: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else
    {
        rtc_gettime( &rtc_time );
        uart0_puts( "  (Task 3) Hora: " );
        uart0_putint( rtc_time.hour );
        uart0_putchar( ':' );
        uart0_putint( rtc_time.min );
        uart0_putchar( ':' );
        uart0_putint( rtc_time.sec );
        uart0_puts( "\n" );
    }
}

void Task4( void )  /* Cada 10 segundos (1000 ticks) muestra por la UART0 los ticks transcurridos */
{
    static boolean init = TRUE;
    static uint32 ticks;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 4: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
        ticks = 0;
    }
    else
    {
        ticks += TICKS_PER_SEC * 10;
        uart0_puts( "  (Task 4) Ticks: " );
        uart0_putint( ticks );
        uart0_puts( "\n" );
    }
}

void Task5( void )  /* Cada vez que reciba un scancode lo muestra por la UART0 */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 5: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else if( flagTask5 )
    {
        flagTask5 = FALSE;
        uart0_puts( "  (Task 5) Tecla pulsada: " );
        uart0_puthex( scancode );
        uart0_puts( "\n" );
    }
}

void Task6( void )  /* Cada vez que reciba un scancode lo muestra por los 7 segmentos */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 6: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else if( flagTask6 )
    {
        flagTask6 = FALSE;
        D8Led_digit( scancode );
    }
}

void Task7( void )  /* Cada vez que se presione un pulsador lo avisa por la UART0 */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 7: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else if( flagPb )
    {
        flagPb = FALSE;
        uart0_puts( "  (Task 7) Se ha pulsado algún pushbutton...\n" );
    }
}

/*******************************************************************/

void isr_pb( void )
{
    flagPb = TRUE;
    rEXTINTPND = (1 << 2) | (1 << 3);
	ic_cleanflag(INT_EINT4567);
}

void isr_tick( void )
{
    flagTimer = TRUE;
    ic_cleanflag(INT_TIMER0);
};

#endif
/*******************************************************************/
