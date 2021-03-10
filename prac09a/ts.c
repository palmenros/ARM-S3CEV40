#include "44b.h"
#include "adc.h"
#include "lcd.h"
#include "gpio.h"
#include "utils.h"
#include "ts.h"
#include "intcontroller.h"
#include "uart.h"

#define PX_ERROR    (5)
#define TS_DOWN_DELAY 100
#define TS_UP_DELAY 200


static uint16 Vxmin = 0;
static uint16 Vxmax = 0;
static uint16 Vymin = 0;
static uint16 Vymax = 0;

static uint8 state;

extern void isr_TS_dummy( void ) __attribute__ ((interrupt ("IRQ")));

void isr_TS_dummy( void )
{
	ic_cleanflag(INT_EINT2);
}

static void ts_scan( uint16 *Vx, uint16 *Vy );
static void ts_calibrate( void );
static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y );

void ts_init( void )
{
    lcd_init();
    adc_init();

    //Conf PE[7..4] as Output
    rPCONE &= ~(0xff << 8);
    rPCONE |= 0b01010101 << 8;

    //Conecta Y‐ con GND dejando el resto de terminales abiertos
    //PE[7..4] = 1011
    rPDATE &= ~(0xf << 4);
    rPDATE |= 0b1011 << 4;

    Delay( 1 );
    portG_conf(2, INPUT);
    //portG_conf_pup(2, ENABLE);

    ts_on();
    ts_calibrate();
}

void ts_on( void )
{
    adc_on();
    state = ON;
}

void ts_off( void )
{
    adc_off();
    state = OFF;
}

uint8 ts_status( void )
{
	return state;
}

uint8 ts_pressed( void )
{
    return !(rPDATG & (1 << 2));
}

static void ts_calibrate( void )
{
    uint16 x, y;

	uart0_puts("Calibracion pantalla tactil: \n");
    lcd_on();
    do {

    	lcd_clear_color(BLACK);
    	lcd_draw_box(0, 0, 0 + PX_ERROR, 0 + PX_ERROR, WHITE, 3);
    	uart0_puts("Pulse el punto en la esquina superior izquierda...");

    	while(rPDATG & (1 << 2));
        Delay( TS_DOWN_DELAY );
        ts_scan( &Vxmin, &Vymax );
    	while(!(rPDATG & (1 << 2)));
        Delay( TS_UP_DELAY );

    	uart0_puts(" Ok\n");

    	lcd_clear_color(BLACK);
    	lcd_draw_box(LCD_WIDTH - 1 - PX_ERROR, LCD_HEIGHT - 1 - PX_ERROR, LCD_WIDTH - 1, LCD_HEIGHT - 1, WHITE, 3);
    	uart0_puts("Pulse el punto en la esquina inferior derecha...");

    	while(rPDATG & (1 << 2));
        Delay( TS_DOWN_DELAY );
        ts_scan( &Vxmax, &Vymin );
    	while(!(rPDATG & (1 << 2)));
        Delay( TS_UP_DELAY );

        uart0_puts(" Ok\n");

    	lcd_clear_color(BLACK);
    	lcd_draw_box(LCD_WIDTH/2 - PX_ERROR / 2, LCD_HEIGHT/2 - PX_ERROR / 2, LCD_WIDTH/2 + PX_ERROR / 2, LCD_HEIGHT/2 + PX_ERROR / 2, WHITE, 3);

    	uart0_puts("Pulse el punto en el centro...");

        ts_getpos( &x, &y );

    	uart0_puts("X: ");
    	uart0_putint(x);
    	uart0_puts(", Y: ");
    	uart0_putint(y);
    	uart0_puts("\n");

    } while( (x > LCD_WIDTH/2+PX_ERROR) || (x < LCD_WIDTH/2-PX_ERROR) || (y > LCD_HEIGHT/2+PX_ERROR) || (y < LCD_HEIGHT/2-PX_ERROR) );

    uart0_puts("Calibracion completada\n");
    lcd_clear();
}

void ts_getpos( uint16 *x, uint16 *y )
{
	uint16 Vx, Vy;

	//Esperar a que PG[2]=0
	while(rPDATG & (1 << 2));

	//Esperar TS_DOWN_DELAY
	Delay(TS_DOWN_DELAY);

	ts_scan(&Vx, &Vy);
	ts_sample2coord(Vx, Vy, x, y);

	//Esperar a que PG[2]=1
	while(!(rPDATG & (1 << 2)));

	//Esperar TS_UP_DELAY
	Delay(TS_UP_DELAY);
}

static void ts_scan( uint16 *Vx, uint16 *Vy )
{
	//PE[7..4] = (0,1,1,0)
    rPDATE &= ~(0xf << 4);
    rPDATE |= 0b0110 << 4;

    //leer Vx por canal AIN1

    *Vx = adc_getSample( ADC_AIN1 );

	//PE[7..4] = (1,0,0,1)
    rPDATE &= ~(0xf << 4);
    rPDATE |= 0b1001 << 4;

    //leer Vy por canal AIN0

    *Vy = adc_getSample( ADC_AIN0 );

	//PE[7..4] = (1,0,1,1)
    rPDATE &= ~(0xf << 4);
    rPDATE |= 0b1011 << 4;

    Delay(1);
}

static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y )
{
	//Interpolate Vx
    if( Vx < Vxmin )
        *x = 0;
    else if( Vx > Vxmax )
        *x = LCD_WIDTH-1;
    else
        *x = LCD_WIDTH*(Vx-Vxmin) / (Vxmax-Vxmin);

    //Interpolate Vy
    if( Vy < Vymin )
        *y = 0;
    else if( Vy > Vymax )
        *y = LCD_HEIGHT-1;
    else
        *y = LCD_HEIGHT*(Vy-Vymin) / (Vymax-Vymin);

    //Vy crece cuando y decrece
    *y = LCD_HEIGHT - *y;
}

void ts_open( void (*isr)(void) )
{
	pISR_EINT2 = (unsigned) isr;
	ic_cleanflag(INT_EINT2);
	ic_enable(INT_EINT2);
}

void ts_close( void )
{
    ic_disable(INT_EINT2);
    pISR_EINT2 = (unsigned) isr_TS_dummy;
}

