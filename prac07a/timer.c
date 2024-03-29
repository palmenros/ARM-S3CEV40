/*********************************************************************************************
* Fichero:		timer.c
* Descrip:		funciones de control del timer0 del s3c44b0x
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "timer.h"

int tmr_set_prescaler(int p, int  value)
{
	int offset = p*8;
	value &= 0xFF;

	if (p < 0 || p > 3)
		return -1;

	//COMPLETAR: escribir el valor value a partir de la posición offset en el
	//registro rTCFG0, para establecer el valor de pre-escalado del módulo p

	int bitfield = value, mask = 0xff, reg = rTCFG0;
	bitfield <<= offset;

	mask <<= offset;
	mask ^= ~0;

	reg &= mask; // Ponemos a 0 todos los valores en la zona que va a ir el nuevo byte
	reg |= bitfield; // Ponemos a 1 los bits que esten a 1 del nuevo byte

	rTCFG0 = reg;

	return 0;
}

int tmr_set_divider(int d, enum tmr_div div)
{
	int pos = d*4;

	if ((d < 0 || d > 5) ||
			(div == D1_32 && d > 3) ||
			(div == EXTCLK && d != 5) ||
			(div == TCLK && d != 4))
		return -1;

	if (div == EXTCLK || div == TCLK)
		div = 4;

	//COMPLETAR: escribir el valor div a partir de la posición pos en el
	//registro rTCFG1 para establecer el valor para el divisor d

	int bitfield = div, mask = 0xf, reg = rTCFG1;
	bitfield <<= pos;

	mask <<= pos;
	mask ^= ~0;

	reg &= mask; // Ponemos a 0 todos los valores en la zona que va a ir el nuevo nibble
	reg |= bitfield; // Ponemos a 1 los bits que esten a 1 del nuevo nibble

	rTCFG1 = reg;

	return 0;
}

int tmr_set_count(enum tmr_timer t, int count, int cmp)
{
	int err = 0;
	switch (t) {
		case TIMER0:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer0 (rTCNTB0 y
			//rTCMPB0)
			rTCNTB0 = count;
			rTCMPB0 = cmp;
			break;
		case TIMER1:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer1 (rTCNTB1 y
			//rTCMPB1)
			rTCNTB1 = count;
			rTCMPB1 = cmp;
			 break;
		case TIMER2:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer2 (rTCNTB2 y
			//rTCMPB2)
			rTCNTB2 = count;
			rTCMPB2 = cmp;
			 break;
		case TIMER3:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer3 (rTCNTB3 y
			//rTCMPB3)
			rTCNTB3 = count;
			rTCMPB3 = cmp;
			 break;
		case TIMER4:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer4 (rTCNTB4 y
			//rTCMPB4)
			rTCNTB4 = count;
			rTCMPB4 = cmp;
			 break;
		case TIMER5:
			//COMPLETAR: establecer el valor de cuenta count en el registro de 
			//buffer del timer5 (rTCNTB5)
			rTCNTB5 = count;
			 break;
		default:
			err = -1;
	}

	return err;
}

int tmr_update(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	pos++;

	//COMPLETAR: poner a 1 en el registro rTCON el bit indicado por pos 
	// y justo después ponerlo a 0 (deben ser stores distintos, lo hacemos con
	// sentencias C distintas)
	rTCON |= 1 << pos;
	rTCON &= ~(1 << pos);

	return 0;
}

int tmr_set_mode(enum tmr_timer t, enum tmr_mode mode)
{
	int err = 0;
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	if(t == 5)
		pos += 2;
	else
		pos += 3;

	if (mode == ONE_SHOT)
		//COMPLETAR: poner a 0 el bit autoreload a partir de la posición pos (es
		//el cuarto bit a partir de esa posición)
		rTCON &= ~(1 << pos);
	else if (mode == RELOAD)
		//COMPLETAR: poner a 1 el bit autoreload a partir de la posición pos (es
		//el cuarto bit a partir de esa posición)
		rTCON |= 1 << pos;
	else
		err = -1;

	return err;
}

int tmr_start(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	//COMPLETAR: poner a 1 el bit de start a partir de la posición pos en el
	//registro rTCON (es el primer bit del grupo)
	rTCON |= 1 << pos;
	return 0;
}

int tmr_stop(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	//COMPLETAR: poner a 0 el bit de start a partir de la posición pos en el
	//registro rTCON (es el primer bit del grupo)
	rTCON &= ~(1 << pos);

	return 0;
}

int tmr_isrunning(enum tmr_timer t)
{
	int ret = 0;
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if ((t >= 0) && (t <= 5) 
			&& (rTCON & (0x1 << pos)))
		ret = 1;

	return ret;
}

