#include "44b.h"
#include "iic.h"
#include "utils.h"

void iic_init( void )
{
	//Slave Address: Undefined, always going to be master
	rIICADD = 0;

	//*250kHz
	//IICCON[7]	  = 1   Generate ACK
	//IICCON[6]   = 0   IICCLK = MCLK / 16
	//IICCON[5]   = 1	Enable RX/TX Interrupts
	//IICCON[3:0] = 15  TxCLK = IICCLK / (15+1) = 64 MHz / 256
	rIICCON = 0xAF;

	//*Transfer mode: set per transfer
	//ICCSTAT[4] = 1 read/write enable
	rIICSTAT = 0x10;
}

void iic_start(enum IIC_Mode_t mode, uint8 addr)
{
	//Address
    rIICDS = addr;

    //Set mode, send START signal, Serial Output enable
    rIICSTAT = mode << 6 | 0b11 << 4;

    //rIICCON[4] = 0  start transmission
    rIICCON &= ~(1 << 4);

    while(!( rIICCON & (1 << 4)));
}


void iic_putByte( uint8 byte )
{
    rIICDS   = byte;

    //rIICCON[4] = 0  start transmission
    rIICCON &= ~(1 << 4);
    while(!( rIICCON & (1 << 4)));
}

uint8 iic_getByte(enum IIC_ACK_t ack)
{
	// Setup ACK or not
	if(ack == RxACK) {
		//IICCON[7] = 1   Generate ACK
	    rIICCON |= 1 << 7;
	} else {
		//IICCON[7] = 0   Don't generate ACK
	    rIICCON &= ~(1 << 7);
	}

    //rIICCON[4] = 0  start transmission
    rIICCON &= ~(1 << 4);
    while(!( rIICCON & (1 << 4)));

    return rIICDS;
}

void iic_stop( uint16 ms )
{
	//Generate STOP signal (ICCSTAT[5]=0)
    rIICSTAT &= ~(1 << 5);

    //rIICCON[4] = 0  start transmission of STOP signal
    rIICCON &= ~(1 << 4);

    Delay(ms);
}

