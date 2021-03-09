#include "44b.h"
#include "iis.h"
#include "dma.h"
#include "intcontroller.h"

static void isr_bdma0( void ) __attribute__ ((interrupt ("IRQ")));

static enum IIS_mode_t iomode;

void iis_init(enum IIS_mode_t mode )
{
    iomode = mode;

    //** Common config */
    //IISMOD[5] = 0 Polaridad de LRCLK: 0 para canal izquierdo
    //IISMOD[4] = 0  IIS compatible
    //IISMOD[3] = 1  16b Transfer
    //IISMOD[2] = 0 Frecuencia de CODECLK: 256fs
    //IISMOD[1:0] = 1 Frecuencia de SCLK: 32fs

    //IISPSR[7:4] = 7 Frecuencia de muestreo: fs = 16 KHz (calidad Voz IP)
    //IISPSR[3:0] = 7 division factor = 16

    //IISCON[3] = 0 Generación de LRCLK en inactividad: sí en transmisión
    //IISCON[2] = 0 Generación de LRCLK en inactividad: sí en recepción

    if( iomode == IIS_POLLING ) {

    	//IISPSR[7:4] = 7 Frecuencia de muestreo: fs = 16 KHz (calidad Voz IP)
    	//IISPSR[3:0] = 7 division factor = 16

    	rIISPSR  = 0x7 | (0x7 << 4);

    	//IISMOD[8] = 0 master mode
		//IISMOD[7:6] = 3 modo transmisión y recepción
		//IISMOD[5] = 0 Polaridad de LRCLK: 0 para canal izquierdo
		//IISMOD[4] = 0  IIS compatible
		//IISMOD[3] = 1  16b Transfer
		//IISMOD[2] = 0 Frecuencia de CODECLK: 256fs
		//IISMOD[1:0] = 1 Frecuencia de SCLK: 32fs

    	rIISMOD  = 0b011001001;

    	//IISFCON[11] = 0 normal access mode (no DMA)
		//IISFCON[10] = 0 normal access mode (no DMA)
		//IISFCON[9] = 1 Tx FIFO enable
		//IISFCON[8] = 1 Rx FIFO enable

        rIISFCON = 0b0011 << 8;

        //IISCON[5] = 0 Tx DMA request disable
		//IISCON[4] = 0 Rx DMA request disable
		//IISCON[3] = 0 Generación de LRCLK en inactividad: sí en transmisión
		//IISCON[2] = 0 Generación de LRCLK en inactividad: sí en recepción
		//IISCON[1] = 1 prescaler habilitado
		//IISCON[0] = 1 interfaz IIS habilitado

        rIISCON  = 0b000011;

    } else if( iomode == IIS_DMA ) {

        //IISPSR[7:4] = 7 Frecuencia de muestreo: fs = 16 KHz (calidad Voz IP)
        //IISPSR[3:0] = 7 division factor = 16

        rIISPSR  = 0x7 | (0x7 << 4);

    	//IISMOD[8] = 0 master mode off
    	//IISMOD[7:6] = 0 no transfer
        //IISMOD[5] = 0 Polaridad de LRCLK: 0 para canal izquierdo
        //IISMOD[4] = 0  IIS compatible
        //IISMOD[3] = 1  16b Transfer
        //IISMOD[2] = 0 Frecuencia de CODECLK: 256fs
        //IISMOD[1:0] = 1 Frecuencia de SCLK: 32fs

        rIISMOD  = 0b000001001;

    	//IISFCON[11] = 1 DMA access mode
    	//IISFCON[10] = 1 DMA access mode
    	//IISFCON[9] = 1 Tx FIFO habilitada
    	//IISFCON[8] = 1 Rx FIFO habilitada

        rIISFCON = 0b1111 << 8;

    	//IISCON[5] = 1 Tx DMA service request habilitada
    	//IISCON[4] = 1 Rx DMA service request habilitada
        //IISCON[3] = 0 Generación de LRCLK en inactividad: sí en transmisión
        //IISCON[2] = 0 Generación de LRCLK en inactividad: sí en recepción
    	//IISCON[1] = 1  prescaler habilitado
    	//IISCON[0] = 0 interfaz IIS deshabilitado

        rIISCON  = 0b110010;

        bdma0_init();
        bdma0_open( isr_bdma0 );
    }
}

static void isr_bdma0( void )
{
	//Disable IIS Controller
    rIISCON &= ~1;

    //Clean flag
    ic_cleanflag(INT_BDMA0);
}

inline void iis_putSample( int16 ch0, int16 ch1 )
{
	//Polling only

	//Wait until there are two empty spots in FIFO
    while(((rIISFCON >> 4) & 0b1111) > 6);

    //Send data
    IISFIF = ch0;
    IISFIF = ch1;
}

inline void iis_getSample( int16 *ch0, int16 *ch1 )
{
	//Polling only

	//Wait until there are two samples in FIFO
    while((rIISFCON & 0b1111) < 2);

    *ch0 = IISFIF;
    *ch1 = IISFIF;
}

void iis_play( int16 *buffer, uint32 length, uint8 loop )
{
    uint32 i;
    int16 ch1, ch2;
    uint8 loop_23_22, loop_21;

    if( iomode == IIS_POLLING )
        for( i=0; i<length/2; )
        {
            ch1 = buffer[i++];
            ch2 = buffer[i++];
            iis_putSample( ch1, ch2 );
        }
    if( iomode == IIS_DMA ){

    	//BDISRC0[31:30] = 1 half word (16b)
    	//BDISRC0[29:28] = 1 post‐incrementada source address
    	//BDISRC0[27:0] = ... dirección del buffer source address

    	rBDISRC0  = (1 << 30) | (1 << 28) | (uint32) buffer;

    	//BDIDES0[31:30] = 1 from external memory to internal peripheral transfer direction
    	//BDIDES0[29:28] = 3 fija (dest address)
    	//BDIDES0[27:0] = ... dirección de IISFIF (dest address)

		rBDIDES0  = (1 << 30) | (3 << 28) | (uint32) &IISFIF;

    	//BDCON0[3:2] = 0 DMA request enable
    	//BDCON0[1:0] = 0 no command
		rBDCON0  = 0;

    	//BDICNT0[31:30] = 1 DMA source seleccion: IIS
    	//BDICNT0[29:28] = 0 DMA handshake mode
    	//BDICNT0[27:26] = 1 DMA unit transfer mode
    	//BDICNT0[25:24] = 0 DMA no on‐the‐fly mode
    	//BDICNT0[23:22] = 0 (no) si loop = TRUE; 3 (sí, al terminar la transferencia DMA) en otro caso
    	//BDICNT0[21] = 1 (activado) si loop = TRUE; 0 (desactivado) en otro caso
    	//BDICNT0[20] = 1 DMA enable
    	//BDICNT0[19:0] = ... tamaño del buffer a transferur

		loop_23_22 = loop ? 0 : 3;
		loop_21 = loop ? 1 : 0;

		rBDICNT0  = (1 << 30) | (1 << 26) | (loop_23_22 << 22) | (loop_21 << 21) | (0xfffff & length);
		rBDICNT0 |= (1 << 20);

    	//*IIS Controller

    	//IISMOD[7:6] = 2 modo transmisión
    	//IISCON[0] = 1 interfaz IIS habilitado
        rIISMOD   = (rIISMOD & ~(3 << 6)) | (2 << 6);
        rIISCON  |= 1;
    }
}

void iis_rec( int16 *buffer, uint32 length )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING ) {
    	//Repeated polling
    	for( i=0; i<length/2; ) {
			iis_getSample( buffer, buffer + 1);
			buffer += 2;
		}
    } else if( iomode == IIS_DMA ) {

    	//Wait while DMA transfer is ongoing
        while( rIISCON & 1  );

        //Set up DMA Receive
        rBDISRC0  = (1 << 30) | (3 << 28) | (uint32)&IISFIF;
        rBDIDES0  = (2 << 30) | (1 << 28) | (uint32) buffer;
        rBDCON0   = 0;
        rBDICNT0  = (1 << 30) | (1 << 26) | (3 << 22) | (0xfffff & length);
        rBDICNT0 |= (1 << 20);

        //*Set up IIS

        //IISMOD[7:6] = 1 modo recepción
        //IISCON[0] = 1 interfaz IIS habilitado

        rIISMOD   = (rIISMOD & ~(3 << 6)) | (1 << 6);
        rIISCON  |= 1;
    }
}

void iis_pause( void )
{
	//Pause DMA transfer

	//Desabilita IIS
	rIISCON &= ~1;
}

void iis_continue( void )
{
	//Continue DMA transfer

	//Habilita IIS
	rIISCON |= 1;
}

uint8 iis_status( void )
{
	//Return if DMA is currently playing / recording
    return rIISCON & 1;
}

void iis_playWawFile( int16 *wav, uint8 loop )
{
    uint32 size;
    char *p;

    p = (char *) wav;
    while ( !(p[0] == 'd' && p[1] == 'a' && p[2] == 't' && p[3] == 'a') ) // busca el chunck data
        p++;
    p += 4;

    size = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24); // los datos de cabecera están en little-endian
    p += 4;

    iis_play( (int16 *)p, size, loop );
}
