#ifndef __DMA_H__
#define __DMA_H__

#include "common_types.h"

//Init BDMA0
void bdma0_init( void );

//Set up BDMA0 ISR
void bdma0_open( void (*isr)(void) );

//Clear BDMA0 ISR
void bdma0_close( void );

#endif
