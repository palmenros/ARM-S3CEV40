#include "common_types.h"
#include "44b.h"
#include "at24c04.h"
#include "iic.h"

#define DEVICE_ADDR  (( 0xA << 4) | (0 << 1))

#define READ  (1)
#define WRITE (0)

void at24c04_clear( void )
{
    //Store 32 pages of 16 bytes zeros
	int page_count = 0, byte_count = 0;
	uint8 bit;

	for(page_count=0; page_count < 32; ++page_count) {
		//Store page
		int addr = page_count * 16;
		bit = (addr & 0x100) >> 8;

		//Se envía la start condition seguida del 1er. byte que incluye:
		//-los 6 bits más significativos de la dirección del dispositivo IIC (101000)
		//-el bit 8 de la dirección del dato
		//-un 0 para indicar la operación de escritura
		iic_start( IIC_Tx, DEVICE_ADDR | (bit << 1) | WRITE );

		//Se envía el 2do. byte que incluye los bits 7..0 (8 bits) de la dirección del dato.
		iic_putByte(addr & 0xff);

		//Se envían del 3er. al 18vo. byte que incluyen los 16 primeros datos (8 bits cada uno).
		for(byte_count = 0; byte_count < 16; byte_count++) {
			iic_putByte(0);
		}

		//Se envía la stop condition
		iic_stop(5);
	}
}

void at24c04_bytewrite( uint16 addr, uint8 data )
{
    uint8 page;

    page = (addr & 0x100) >> 8;

    iic_start( IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE );
    iic_putByte( addr & 0xFF );
    iic_putByte( data );
    iic_stop( 5 );
}

void at24c04_byteread( uint16 addr, uint8 *data )
{
	//Read random byte
	uint8 page;
    page = (addr & 0x100) >> 8;

	//Steps:
	//Se envía la start condition seguida del 1er. byte que incluye:
	//- los 6 bits más significativos de la dirección del dispositivo IIC (101000)
	//- el bit 8 de la dirección del dato
	//- un 0 para indicar la operación de escritura (escritura ficticia)
    iic_start( IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE );

	//Se envía el 2do. byte que incluye los bits 7..0 (8 bits) de la dirección del dato.
    iic_putByte( addr & 0xFF );

	//Se envía la start condition seguida del 3er. byte que incluye:
	//- La misma información que el 1er. byte pero con un 1 final para indicar la operación de lectura
    iic_start( IIC_Rx, DEVICE_ADDR | (page << 1) | READ);

	//Se recibe el byte que incluye el dato (sin enviar ACK).
    *data = iic_getByte(NO_RxACK);

	//Se envía la stop condition
    iic_stop( 5 );
}

void at24c04_load( uint8 *buffer )
{
    //Load sequentially all 512 bytes from 0x0
	int i = 0;

	//Se envía la start condition seguida del 1er. byte que incluye:
	// - los 6 bits más significativos de la dirección del dispositivo IIC (101000)
	// - el bit 8 de la dirección del dato
	// -un 0 para indicar la operación de escritura (escritura ficticia)
    iic_start( IIC_Tx, DEVICE_ADDR | (0 << 1) | WRITE );

	//Se envía el 2do. byte que incluye los bits 7..0 (8 bits) de la dirección del dato.
    iic_putByte(00);

	//Se envía la start condition seguida del 3er. byte que incluye:
	// - La misma información que el 1er. byte pero con un 1 final para indicar la operación de lectura
    iic_start( IIC_Rx, DEVICE_ADDR | (0 << 1) | READ );

	//Se reciben n‐1 bytes que incluyen los datos (enviando ACK en cada uno)
    for(i = 0; i < 512 - 1; ++i) {
    	*buffer = iic_getByte(RxACK);
    	buffer++;
    }

	//Se recibe el último byte que incluye datos (sin enviar ACK).
	*buffer = iic_getByte(NO_RxACK);

	//Se envía la stop condition.
    iic_stop( 5 );
}

void at24c04_store( uint8 *buffer )
{
    //32 stores of 16 byte pages
	int page_count = 0, byte_count = 0;
	uint8 bit;

	for(page_count=0; page_count < 32; ++page_count) {
		//Store page
		int addr = page_count * 16;
	    bit = (addr & 0x100) >> 8;

	    //Se envía la start condition seguida del 1er. byte que incluye:
		//-los 6 bits más significativos de la dirección del dispositivo IIC (101000)
		//-el bit 8 de la dirección del dato
		//-un 0 para indicar la operación de escritura
	    iic_start( IIC_Tx, DEVICE_ADDR | (bit << 1) | WRITE );

	    //Se envía el 2do. byte que incluye los bits 7..0 (8 bits) de la dirección del dato.
	    iic_putByte(addr & 0xff);

	    //Se envían del 3er. al 18vo. byte que incluyen los 16 primeros datos (8 bits cada uno).
	    for(byte_count = 0; byte_count < 16; byte_count++) {
	    	iic_putByte(*buffer);
	    	++buffer;
	    }

	    //Se envía la stop condition
	    iic_stop(5);
	}
}

