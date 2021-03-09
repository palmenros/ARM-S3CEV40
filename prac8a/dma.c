#include "44b.h"
#include "dma.h"
#include "intcontroller.h"

void isr_BDMA0_dummy( void )  __attribute__ ((interrupt ("IRQ")));

void isr_BDMA0_dummy( void ) {
	ic_cleanflag(INT_BDMA0);
}

void bdma0_init( void )
{
    rBDCON0  = 0;
    rBDISRC0 = 0;
    rBDIDES0 = 0;
    rBDICNT0 = 0;

    ic_conf_line(INT_BDMA0, IRQ);
}

void bdma0_open( void (*isr)(void) )
{
    pISR_BDMA0 = (unsigned) isr;
    ic_cleanflag(INT_BDMA0);
    ic_enable(INT_BDMA0);
}

void bdma0_close( void )
{
    ic_disable(INT_BDMA0);
    pISR_BDMA0 = (unsigned) isr_BDMA0_dummy;
}

