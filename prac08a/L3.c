#include "44b.h"
#include "l3.h"
#include "leds.h"
#include "gpio.h"

#define SHORT_DELAY { volatile int8 j; for( j=0; j<4; j++ ); }

void L3_init( void )
{
    //L3CLOCK = 1
	//L3MODE = 1
    uint8 rled, lled;

    rled = !led2_status();
    lled = !led1_status();

    portB_conf(5, OUTPUT);
    portB_conf(4, OUTPUT);
    rPCONA &= ~(1 << 9);

    //L3CLOCK = PDATB[5]
    //L3MODE = PDATB[4]

    rPDATB = (rled << 10) | (lled << 9) | (1 << 5) | (1 << 4);
}

void L3_putByte( uint8 byte, enum L3_mode_t mode )
{
    uint8 i;
    uint8 rled, lled;

    rled = !led2_status();
    lled = !led1_status();

    //L3CLOCK = PDATB[5]
    //L3MODE = PDATB[4]

    //L3CLOCK=1, L3MODE=mode
    rPDATB = (rled << 10) | (lled << 9) | (1 << 5) | (mode << 4);
    SHORT_DELAY;

    for( i=0; i<8; i++ )
    {
    	//L3CLOCK = 0, L3MODE = mode
        rPDATB = (rled << 10) | (lled << 9) | (0 << 5) | (mode << 4);

        //L3DATA = byte[i]
        //L3DATA = PDATA[9]
        rPDATA &= ~(1 << 9);
        rPDATA |= (byte & 1) << 9;
        byte >>= 1;

        SHORT_DELAY;

        //L3CLOCK = 1, L3MODE = mode
        rPDATB = (rled << 10) | (lled << 9) | (1 << 5) | (mode << 4);
        SHORT_DELAY;
    }

    rPDATB = (rled << 10) | (lled << 9) | (1 << 5) | (1 << 4);
}

