/*-------------------------------------------------------------------
**
**  Fichero:
**    utils.h  10/6/2014
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    auxiliares
**
**  Notas de diseño:
**    Equivale al fichero homónimo usado en Fundamentos de Computadores 
**
**-----------------------------------------------------------------*/

#ifndef UTILS_H_
#define UTILS_H_

#define NOP \
  asm volatile ( "nop" );

#define SWI( num ) \
  asm volatile ( "swi %0" : : "i" (num) )

#define GET_SWI_NUMBER( num_p )                          \
  asm volatile ( "ldr r0, [lr, #-4]" : : : "r0" );       \
  asm volatile ( "bic r0, r0, #0xff000000" : : : "r0" ); \
  asm volatile ( "str r0, %0" : "=m" (*num_p) : : );

#define INT_DISABLE \
  asm volatile( "mrs   r0, cpsr" : : : "r0" ); \
  asm volatile( "stmfd sp!, {r0}" ); \
  asm volatile( "orr   r0, r0, #0b11000000" ); \
  asm volatile( "msr   cpsr_c, r0" );

#define INT_ENABLE \
    asm volatile( "ldmfd sp!, {r0}" : : : "r0" ); \
	asm volatile( "msr   cpsr_c, r0" );

#define IRQ_NESTING_ENABLE                      \
  asm volatile ( "mrs   lr, spsr" );            \
  asm volatile ( "stmfd sp!, {lr}" );           \
  asm volatile ( "msr   cpsr_c, #0b00011111" ); \
  asm volatile ( "stmfd sp!, {lr}" );

#define IRQ_NESTING_DISABLE                     \
  asm volatile ( "ldmfd sp!, {lr}" );           \
  asm volatile ( "msr   cpsr_c, #0b10010010" ); \
  asm volatile ( "ldmfd sp!, {lr}" );           \
  asm volatile ( "msr   spsr, lr" );

#define LOCK_MUTEX( mutex_p )                               \
  asm ( "mov r0, #1" : : : "r0" );                 \
  asm ( "swpb r0, r0, [%0]" : : "r" (mutex_p) : ); \
  asm ( "cmp r0, #1" );                            \
  asm ( "subeq pc, pc, #20")

#define UNLOCK_MUTEX( mutex_p )                            \
  asm ( "mov r0, #0" : : : "r0" );                \
  asm ( "strb r0, [%0]" : : "r" (mutex_p) )

#define MEMBAR                          \
  asm volatile ( "" : : : "memory" );

void Delay( int time );
inline void sleep(void);

#endif
