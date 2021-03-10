#include "l3.h"
#include "44b.h"
#include "uda1341ts.h"

#define ADDRESS (0x05)

#define DATA0  (0x0)
#define DATA1  (0x1)
#define STATUS (0x2)

#define EA (0x18 << 3)
#define ED (0x7 << 5)

static uint8 volume;
static uint8 state;

void uda1341ts_init( void )
{
    L3_init();

    L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
    L3_putByte( (1 << 6) | (2 << 4), L3_DATA_MODE );
    L3_putByte( (2 << 4), L3_DATA_MODE );

    L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE  );
    L3_putByte( EA | (2), L3_DATA_MODE );
    L3_putByte( ED | 1, L3_DATA_MODE );

    uda1341ts_setvol( VOL_MAX );

    uda1341ts_on( UDA_DAC );
    uda1341ts_on( UDA_ADC );
}

void uda1341ts_mute( uint8 on )
{
	//DATA0[7:6] = 2 y DATA0[2]=1/0 (0 no mute, 1 mute)
    L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE );
    L3_putByte( (0x2 << 6) | ((on & 1) << 2) , L3_DATA_MODE);
}

void uda1341ts_on( enum UDA_converter_t converter )
{
    state |= converter;

    //Para encender/apagar el ADC/DAC con 6dB de ganancia de entrada
    //STATUS[7]=1 y STATUS[1:0]=PC valor según corresponda
    //PC1 PC0 ADC DAC
    //  0   0 off off
    //  0   1 off  on
    //  1   0  on off
    //  1   1  on  on

    //STATUS[6]=1 y STATUS[5]=1 6dB de ganancia
    //STATUS[4]=0 y STATUS[3]=0 non inverting ADC/DAC polarity
    //STATUS[2]=0 no double speed

    L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
    L3_putByte( (0x1 << 7) | (0b11000 << 2) | state , L3_DATA_MODE);
}

void uda1341ts_off( enum UDA_converter_t converter )
{
    state &= ~converter;

    //Para encender/apagar el ADC/DAC con 6dB de ganancia de entrada
    //STATUS[7]=1 y STATUS[1:0]=X valor según corresponda
    //STATUS[6]=1 y STATUS[5]=1 6dB de ganancia
    //STATUS[4]=0 y STATUS[3]=0 non inverting ADC/DAC polarity
    //STATUS[2]=0 no double speed

    L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
    L3_putByte( (0x1 << 7) | (0b11000 << 2) | state , L3_DATA_MODE);
}

uint8 uda1341ts_status( enum UDA_converter_t converter )
{
    return state;
}

void uda1341ts_setvol( uint8 vol )
{
	volume = vol;

	//DATA0[7:6] = 0 y DATA0[5:0]=atenuación (a mayor atenuación, menor volumen)
    L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE );
    L3_putByte( 0x3f - (vol & 0x3f) , L3_DATA_MODE );
}

uint8 uda1341ts_getvol( void )
{
    return volume;
}

