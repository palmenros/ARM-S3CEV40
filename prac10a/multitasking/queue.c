/*-------------------------------------------------------------------
**
**  Fichero:
**    lab15.c  14/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Ejemplo de una aplicación guiada por tiempo con arquitectura
**    cola de funciones tipo FIFO
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
#include "queue.h"

#define TICKS_PER_SEC   (100)

/* Declaración de fifo de punteros a funciones */

#define BUFFER_LEN   (512)

typedef void (*pf_t)(void);

typedef struct fifo {
    uint16 head;
    uint16 tail;
    uint16 size;
    pf_t buffer[BUFFER_LEN];
} fifo_t;

void fifo_init( void );
void fifo_enqueue( pf_t pf );
pf_t fifo_dequeue( void );
boolean fifo_is_empty( void );
boolean fifo_is_full( void );

/* Declaración de recursos */

volatile fifo_t fifo;

uint8 scancode;

/* Declaración de tareas */

void Task1( void );
void Task2( void );
void Task3( void );
void Task4( void );
void Task5( void );
void Task6( void );
void Task7( void );

/* Declaración de RTI */

void isr_pb( void ) __attribute__ ((interrupt ("IRQ")));
void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));

/*******************************************************************/

void queue( void )
{
    pf_t pf;

    /*timers_init();
    uart0_init();
    leds_init();
    segs_init();
    rtc_init();
    pbs_init();
    keypad_init();*/

    uart0_puts( "\n\n Ejecutando una aplicación como cola de funciones\n" );
    uart0_puts( " ------------------------------------------------\n\n" ) ;

    fifo_init();             /* Inicializa cola de funciones */

    Task1();    /* Ejecuta por primera vez a las funciones para inicializarlas */
    Task2();
    Task3();
    Task4();
    Task5();
    Task6();
    Task7();

    pISR_EINT4567 = (unsigned) isr_pb;
    ic_enable(INT_EINT4567);

    //10ms
    tmr_set_mode(TIMER0, RELOAD);
	tmr_set_prescaler(TIMER0, 1);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 40000, 1);

	tmr_update(TIMER0);

	pISR_TIMER0 =(unsigned)isr_tick;
	tmr_start(TIMER0);

	ic_enable(INT_TIMER0);

    while( 1 )
    {
        sleep();                        /* Entra en estado IDLE, sale por interrupción */
        while( !fifo_is_empty() )
        {
            pf = fifo_dequeue();
            (*pf)();                    /* Las tareas encoladas se ejecutan en esta hebra (background) en orden de encolado */
        }
    }

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
                fifo_enqueue( Task5 );
                fifo_enqueue( Task6 );
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
    else
    {
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
    else
    {
    	D8Led_digit(scancode);
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
    else
    {
        uart0_puts( "  (Task 7) Se ha pulsado algún pushbutton...\n" );
    }
}

/*******************************************************************/

void isr_pb( void )
{
    fifo_enqueue( Task7 );
    rEXTINTPND = (1 << 2) | (1 << 3);
	ic_cleanflag(INT_EINT4567);
}


void isr_tick( void )
{
    static uint16 cont5ticks    = 5;
    static uint16 cont50ticks   = 50;
    static uint16 cont100ticks  = 100;
    static uint16 cont1000ticks = 1000;

    if( !(--cont5ticks) )
    {
        cont5ticks = 5;
        fifo_enqueue( Task2 );
    }
    if( !(--cont50ticks) )
    {
        cont50ticks = 50;
        fifo_enqueue( Task1 );
    }
    if( !(--cont100ticks) )
    {
        cont100ticks = 100;
        fifo_enqueue( Task3 );
    }
    if( !(--cont1000ticks) )
    {
        cont1000ticks = 1000;
        fifo_enqueue( Task4 );
    }

    ic_cleanflag(INT_TIMER0);
};

#endif

/*******************************************************************/

void fifo_init( void )
{
    fifo.head = 0;
    fifo.tail = 0;
    fifo.size = 0;
}

void fifo_enqueue( pf_t pf )
{
    fifo.buffer[fifo.tail++] = pf;
    if( fifo.tail == BUFFER_LEN )
        fifo.tail = 0;
    INT_DISABLE;
    fifo.size++;
    INT_ENABLE;
}

pf_t fifo_dequeue( void )
{
    pf_t pf;

    pf = fifo.buffer[fifo.head++];
    if( fifo.head == BUFFER_LEN )
        fifo.head = 0;
    INT_DISABLE;
    fifo.size--;
    INT_ENABLE;
    return pf;
}

boolean fifo_is_empty( void )
{
    return (fifo.size == 0);
}

boolean fifo_is_full( void )
{
    return (fifo.size == BUFFER_LEN-1);
}

/*******************************************************************/
