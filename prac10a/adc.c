#include "intcontroller.h"
#include "44b.h"
#include "adc.h"
#include "utils.h"

static uint8 state;

void isr_ADC_dummy( void ) __attribute__ ((interrupt ("IRQ")));

void isr_ADC_dummy(void)
{
	ic_cleanflag(INT_ADC);
}

void adc_init( void )
{
	//Tasa de conversión: máxima (100 KSPS, tiempo de conversión 10 us)
	//100KHz = 64 MHz / 2(n+1)×16 => n = 19
	rADCPSR = 19;
    adc_off();
}

void adc_on( void )
{
	//	ADCCON[5] = 0 Normal mode
	rADCCON &= ~(1 << 5);

	//Esperar 10 ms antes de arrancar la conversión
	Delay(10);
    state = ON;
}

void adc_off( void )
{
	//ADCCON[5] = 1  Sleep mode
	rADCCON |= 1 << 5;
	state = OFF;
}

uint8 adc_status( void )
{
    return state;
}

uint16 adc_getSample( enum ADC_channel_t ch )
{
    uint32 sample;
    uint8 i;

    //Seleccionar el canal
    rADCCON &= ~(0b111 << 2);
    rADCCON |= ch << 2;

    Delay(10);

    // Hacer la media de 5 samples para estabilizar
    for( i=0, sample=0; i<5; i++ )
    {
    	//ADCCON[1] = 0 deshabilita arranque por lectura
    	//ADCCON[0] = 1 arranca manualmente la conversión

    	rADCCON &= ~2;
        rADCCON |= 1;

    	//Espera hasta que la conversion comience (mientras ADCCON[0] == 1)
        while( rADCCON & 1 );

    	//Espera hasta que la conversion finalice (mientras ADCCON[6] == 0)
        while( !(rADCCON & (1 << 6)) );

        sample += rADCDAT & 0x3ff;
    }
    return sample / 5;
}

void adc_open( void (*isr)(void) )
{
	pISR_ADC = (unsigned) isr;
    ic_enable(INT_ADC);
}

void adc_close( void )
{
    ic_disable(INT_ADC);
    pISR_ADC = (unsigned) isr_ADC_dummy;
}

