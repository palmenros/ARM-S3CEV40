#include "44b.h"
#include "gpio.h"

/* Port B interface implementation */

int portB_conf(int pin, enum port_mode mode)
{
	int ret = 0;
	if (pin < 0 || pin > 10)
		return -1; // indica error

	if (mode == SIGOUT)
		// COMPLETAR: poner en rPCONB el bit indicado por pin a 1 para que por
		// dicho pin en el puerto B salga la señal correspondiente del
		// controlador de memoria
		rPCONB |= 1 << pin;
	else if (mode == OUTPUT)
		// COMPLETAR: poner en rPCONB el bit indicado por pin a 0 para que dicho
		// pin sea un pin de salida
		rPCONB &= ~(1 << pin);
	else
		ret = -1; // indica error

	return ret;
}

int portB_write(int pin, enum digital val)
{
	if (pin < 0 || pin > 10)
		return -1; // indica error

	if (val < 0 || val > 1)
		return -1; // indica error

	if (val)
		// COMPLETAR: poner en rPDATB el bit indicado por pin a 1
		rPDATB |= 1 << pin;
	else
		// COMPLETAR: poner en rPDATB el bit indicado por pin a 0
		rPDATB &= ~(1 << pin);

	return 0;
}

/* Port G interface implementation */

int portG_conf(int pin, enum port_mode mode)
{
	int pos  = pin*2;

	if (pin < 0 || pin > 7)
		return -1; // indica error

	switch (mode) {
		case INPUT:
			// COMPLETAR: poner en rPCONG 00 a partir de la posición pos para
			// configurar como pin de entrada el pin indicado por el parámetro pin
			rPCONG &= ~(1 << pos);  	 // 0
			rPCONG &= ~(1 << (pos + 1)); // 0
			break;
		case OUTPUT:
			// COMPLETAR: poner en rPCONG 01 a partir de la posición pos para
			// configurar como pin de salida el pin indicado por el parámetro pin
			rPCONG |= 1 << pos;			 // 1
			rPCONG &= ~(1 << (pos + 1)); // 0
			break;
		case SIGOUT:
			// COMPLETAR: poner en rPCONG 10 a partir de la posición pos para
			// que salga la señal interna correspondiente por el pin indicado
			// por el parámetro pin
			rPCONG &= ~(1 << pos); 		// 0
			rPCONG |= 1 << (pos + 1);	// 1
			break;
		case EINT:
			// COMPLETAR: poner en rPCONG 11 a partir de la posición pos para
			// habilitar la generación de interrupciones externas por el pin
			// indicado por el parámetro pin
			rPCONG |= 1 << pos;			// 1
			rPCONG |= 1 << (pos + 1);	// 1
			break;
		default:
			return -1;
	}

	return 0;
}

int portG_eint_trig(int pin, enum trigger trig)
{
	int pos = pin*4;

	if (pin < 0 || pin > 7)
		return -1;

switch (trig) {
		case LLOW:
			// COMPLETAR: poner en rEXTINT a partir de la posición pos tres bits
			// a 000, para configurar interrupciones externas por nivel bajo
			rEXTINT &= ~(1 << pos);	 		// 0
			rEXTINT &= ~(1 << (pos + 1));	// 0
			rEXTINT &= ~(1 << (pos + 2));	// 0
			break;
		case LHIGH:
			// COMPLETAR: poner en rEXTINT a partir de la posición pos tres bits
			// a 001, para configurar interrupciones externas por nivel alto
			rEXTINT |= 1 << pos;	 		// 1
			rEXTINT &= ~(1 << (pos + 1));	// 0
			rEXTINT &= ~(1 << (pos + 2));	// 0
			break;
		case FALLING:
			// COMPLETAR: poner en rEXTINT a partir de la posición pos tres bits
			// a 010, para configurar interrupciones externas por flanco de
			// bajada
			rEXTINT &= ~(1 << pos);	 		// 0
			rEXTINT |= 1 << (pos + 1);	 	// 1
			rEXTINT &= ~(1 << (pos + 2));	// 0
			break;
		case RISING:
			// COMPLETAR: poner en rEXTINT a partir de la posición pos tres bits
			// a 100, para configurar interrupciones externas por flanco de
			// subida
			rEXTINT &= ~(1 << pos);	 		// 0
			rEXTINT &= ~(1 << (pos + 1));	// 0
			rEXTINT |= 1 << (pos + 2);	 	// 1
			break;
		case EDGE:
			// COMPLETAR: poner en rEXTINT a partir de la posición pos tres bits
			// a 110, para configurar interrupciones externas por cualquier
			// flanco
			rEXTINT &= ~(1 << pos);	 		// 0
			rEXTINT |= 1 << (pos + 1);	 	// 1
			rEXTINT |= 1 << (pos + 2);	 	// 1
			break;
		default:
			return -1;
	}
	return 0;
}

int portG_write(int pin, enum digital val)
{
	int pos = pin*2;

		if (pin < 0 || pin > 7)
			return -1; // indica error

		if (val < 0 || val > 1)
			return -1; // indica error

		if ((rPCONG & (0x3 << pos)) != (0x1 << pos))
			return -1; // indica error

		if (val)
			// COMPLETAR: poner en rPDATG el bit indicado por pin a 1
			rPDATG |= 1 << pin;
		else
			// COMPLETAR: poner en rPDATG el bit indicado por pin a 0
			rPDATG &= ~(1 << pin);

		return 0;
}

int portG_read(int pin, enum digital* val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1; // indica error

	if (rPCONG & (0x3 << pos))
		return -1; // indica error

	/*COMPLETAR: true si está a 1 en rPDATG el pin indicado por el parámetro pin*/
	if (rPDATG & (1 << pin))
		*val = HIGH;
	else
		*val = LOW;

	return 0;
}

int portG_conf_pup(int pin, enum enable st)
{
	if (pin < 0 || pin > 7)
		return -1; // indica error

	if (st != ENABLE && st != DISABLE)
		return -1; // indica error

	if (st == ENABLE)
		// COMPLETAR: poner el pin de rPUPG indicado por el parametro pin al valor adecuado,
		// para activar la resistencia de pull-up
		rPUPG &= ~(1 << pin);
	else
		// COMPLETAR: poner el pin de rPUPG indicado por el parametro pin al valor adecuado,
		// para desactivar la resistencia de pull-up
		rPUPG |= 1 << pin;

	return 0;
}
