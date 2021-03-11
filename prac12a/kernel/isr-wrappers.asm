/*-----------------------------------------------------------------
**
**  Fichero:
**    lab12-isr-wrappers.asm  24/1/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Implementa wrappers para cada una de las RTI de la aplicación
**    ejecutada bajo el kernel de tiempo real uC/OS-II 
**
**  Notas de diseño:
**    - El wrapper para timer0 está implementado en os_cpu_a.asm
**
**---------------------------------------------------------------*/

//.include "os_port/os_cpu_isr_wrapper.S"

/*-----------------------------------------------------------------
**
**  Fichero:
**    os_cpu_isr_wrapper.asm  20/1/2017
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Implementa una macro que permite añadir un wrapper a una RTI
**    de una aplicación ejecutada bajo el kernel de tiempo real uC/OS-II
**
**  Notas de diseño:
**    - Implementado según las especificaciones del capítulo 3 del
**      libro "MicroC/OS-II. The Real-Time Kernel (2nd. edition)"
**      de Jean J. Labrosse
**    - Asume que interrumpen por IRQ
**
**---------------------------------------------------------------*/

#-----------------------------------------------------------------
#
#  Fichero:
#    s3c44b0x.asm  24/4/2013
#
#    (c) J.M. Mendias
#    Programación de Sistemas y Dispositivos
#    Facultad de Informática. Universidad Complutense de Madrid
#
#  Propósito:
#    Creación de nemotécnicos para los registros de los
#    controladores de dispositivos internos del chip S3C44BOX
#
#  Notas de diseño:
#    Las direcciones usadas asumen un alineamiento little-endian
#
#-----------------------------------------------------------------

/* Controlador de memoria */
.equ BWSCON,    0x1c80000
.equ BANKCON0,  0x1c80004
.equ BANKCON1,  0x1c80008
.equ BANKCON2,  0x1c8000c
.equ BANKCON3,  0x1c80010
.equ BANKCON4,  0x1c80014
.equ BANKCON5,  0x1c80018
.equ BANKCON6,  0x1c8001c
.equ BANKCON7,  0x1c80020
.equ REFRESH,   0x1c80024
.equ BANKSIZE,  0x1c80028
.equ MRSRB6,    0x1c8002c
.equ MRSRB7,    0x1c80030

/* Gestor de reloj y energía */
.equ PLLCON,    0x1d80000
.equ CLKCON,    0x1d80004
.equ CLKSLOW,   0x1d80008
.equ LOCKTIME,  0x1d8000c

/* Controlador de cache */
.equ SYSCFG,    0x1c00000
.equ NCACHBE0,  0x1c00004
.equ NCACHBE1,  0x1c00008

/* Árbitro de bus */
.equ SBUSCON,   0x1c40000

/* Controlador de DMA */
.equ ZDCON0,    0x1e80000 /* canal ZDMA 0 */
.equ ZDISRC0,   0x1e80004
.equ ZDIDES0,   0x1e80008
.equ ZDICNT0,   0x1e8000c
.equ ZDCSRC0,   0x1e80010
.equ ZDCDES0,   0x1e80014
.equ ZDCCNT0,   0x1e80018
.equ ZDCON1,    0x1e80020 /* canal ZDMA 1 */
.equ ZDISRC1,   0x1e80024
.equ ZDIDES1,   0x1e80028
.equ ZDICNT1,   0x1e8002c
.equ ZDCSRC1,   0x1e80030
.equ ZDCDES1,   0x1e80034
.equ ZDCCNT1,   0x1e80038
.equ BDCON0,    0x1f80000 /* canal BDMA 0 */
.equ BDISRC0,   0x1f80004
.equ BDIDES0,   0x1f80008
.equ BDICNT0,   0x1f8000c
.equ BDCSRC0,   0x1f80010
.equ BDCDES0,   0x1f80014
.equ BDCCNT0,   0x1f80018
.equ BDCON1,    0x1f80020 /* canal BDMA 1 */
.equ BDISRC1,   0x1f80024
.equ BDIDES1,   0x1f80028
.equ BDICNT1,   0x1f8002c
.equ BDCSRC1,   0x1f80030
.equ BDCDES1,   0x1f80034
.equ BDCCNT1,   0x1f80038

/* Controlador de puertos de E/S */
.equ SPUCR,     0x1d2004c
.equ PCONA,     0x1d20000 /* puerto A */
.equ PDATA,     0x1d20004
.equ PCONB,     0x1d20008 /* puerto B */
.equ PDATB,     0x1d2000c
.equ PCONC,     0x1d20010 /* puerto C */
.equ PDATC,     0x1d20014
.equ PUPC,      0x1d20018
.equ PCOND,     0x1d2001c /* puerto D */
.equ PDATD,     0x1d20020
.equ PUPD,      0x1d20024
.equ PCONE,     0x1d20028 /* puerto E */
.equ PDATE,     0x1d2002c
.equ PUPE,      0x1d20030
.equ PCONF,     0x1d20034 /* puerto F */
.equ PDATF,     0x1d20038
.equ PUPF,      0x1d2003c
.equ PCONG,     0x1d20040 /* puerto G */
.equ PDATG,     0x1d20044
.equ PUPG,      0x1d20048
.equ EXTINT,    0x1d20050
.equ EXTINTPND, 0x1d20054

/* Bits de EXTINTPND */
.equ BIT_EINT4,  (1<<0)
.equ BIT_EINT5,  (1<<1)
.equ BIT_EINT6,  (1<<2)
.equ BIT_EINT7,  (1<<3)

/* Temporizadores */
.equ TCFG0,     0x1d50000
.equ TCFG1,     0x1d50004
.equ TCON,      0x1d50008
.equ TCNTB0,    0x1d5000c /* temporizador 0 */
.equ TCMPB0,    0x1d50010
.equ TCNTO0,    0x1d50014
.equ TCNTB1,    0x1d50018 /* temporizador 1 */
.equ TCMPB1,    0x1d5001c
.equ TCNTO1,    0x1d50020
.equ TCNTB2,    0x1d50024 /* temporizador 2 */
.equ TCMPB2,    0x1d50028
.equ TCNTO2,    0x1d5002c
.equ TCNTB3,    0x1d50030 /* temporizador 3 */
.equ TCMPB3,    0x1d50034
.equ TCNTO3,    0x1d50038
.equ TCNTB4,    0x1d5003c /* temporizador 4 */
.equ TCMPB4,    0x1d50040
.equ TCNTO4,    0x1d50044
.equ TCNTB5,    0x1d50048 /* temporizador 5 */
.equ TCNTO5,    0x1d5004c

/* UART */
.equ ULCON0,    0x1d00000 /* canal 0 */
.equ UCON0,     0x1d00004
.equ UFCON0,    0x1d00008
.equ UMCON0,    0x1d0000c
.equ UTRSTAT0,  0x1d00010
.equ UERSTAT0,  0x1d00014
.equ UFSTAT0,   0x1d00018
.equ UMSTAT0,   0x1d0001c
.equ UTXH0,     0x1d00020
.equ URXH0,     0x1d00024
.equ UBRDIV0,   0x1d00028
.equ ULCON1,    0x1d04000 /* canal 1 */
.equ UCON1,     0x1d04004
.equ UFCON1,    0x1d04008
.equ UMCON1,    0x1d0400c
.equ UTRSTAT1,  0x1d04010
.equ UERSTAT1,  0x1d04014
.equ UFSTAT1,   0x1d04018
.equ UMSTAT1,   0x1d0401c
.equ UTXH1,     0x1d04020
.equ URXH1,     0x1d04024
.equ UBRDIV1,   0x1d04028

/* Controlador de interrupciones */
.equ INTCON,    0x1e00000
.equ INTPND,    0x1e00004
.equ INTMOD,    0x1e00008
.equ INTMSK,    0x1e0000c
.equ I_PSLV,    0x1e00010
.equ I_PMST,    0x1e00014
.equ I_CSLV,    0x1e00018
.equ I_CMST,    0x1e0001c
.equ I_ISPR,    0x1e00020
.equ I_ISPC,    0x1e00024
.equ F_ISPC,    0x1e0003c

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
.equ BIT_ADC,    (1<<0)
.equ BIT_RTC,    (1<<1)
.equ BIT_UTXD1,  (1<<2)
.equ BIT_UTXD0,  (1<<3)
.equ BIT_SIO,    (1<<4)
.equ BIT_IIC,    (1<<5)
.equ BIT_URXD1,  (1<<6)
.equ BIT_URXD0,  (1<<7)
.equ BIT_TIMER5, (1<<8)
.equ BIT_TIMER4, (1<<9)
.equ BIT_TIMER3, (1<<10)
.equ BIT_TIMER2, (1<<11)
.equ BIT_TIMER1, (1<<12)
.equ BIT_TIMER0, (1<<13)
.equ BIT_UERR01, (1<<14)
.equ BIT_WDT,    (1<<15)
.equ BIT_BDMA1,  (1<<16)
.equ BIT_BDMA0,  (1<<17)
.equ BIT_ZDMA1,  (1<<18)
.equ BIT_ZDMA0,  (1<<19)
.equ BIT_TICK,   (1<<20)
.equ BIT_EINT4567, (1<<21)
.equ BIT_EINT3,  (1<<22)
.equ BIT_EINT2,  (1<<23)
.equ BIT_EINT1,  (1<<24)
.equ BIT_EINT0,  (1<<25)
.equ BIT_GLOBAL, (1<<26)

/* Controlador de LCD */
.equ LCDCON1,   0x1f00000
.equ LCDCON2,   0x1f00004
.equ LCDCON3,   0x1f00040
.equ LCDSADDR1, 0x1f00008
.equ LCDSADDR2, 0x1f0000c
.equ LCDSADDR3, 0x1f00010
.equ REDLUT,    0x1f00014
.equ GREENLUT,  0x1f00018
.equ BLUELUT,   0x1f0001c
.equ DP1_2,     0x1f00020
.equ DP4_7,     0x1f00024
.equ DP3_5,     0x1f00028
.equ DP2_3,     0x1f0002c
.equ DP5_7,     0x1f00030
.equ DP3_4,     0x1f00034
.equ DP4_5,     0x1f00038
.equ DP6_7,     0x1f0003c
.equ DITHMODE,  0x1f00044

/* Conversor analógico digital */
.equ ADCCON,    0x1d40000
.equ ADCPSR,    0x1d40004
.equ ADCDAT,    0x1d40008

/* Reloj de tiempo real */
.equ RTCCON,    0x1d70040
.equ RTCALM,    0x1d70050
.equ ALMSEC,    0x1d70054
.equ ALMMIN,    0x1d70058
.equ ALMHOUR,   0x1d7005c
.equ ALMDAY,    0x1d70060
.equ ALMMON,    0x1d70064
.equ ALMYEAR,   0x1d70068
.equ RTCRST,    0x1d7006c
.equ BCDSEC,    0x1d70070
.equ BCDMIN,    0x1d70074
.equ BCDHOUR,   0x1d70078
.equ BCDDAY,    0x1d7007c
.equ BCDDATE,   0x1d70080
.equ BCDMON,    0x1d70084
.equ BCDYEAR,   0x1d70088
.equ TICNT,     0x1d7008c

/* Watchdog timer */
.equ WTCON,     0x1d30000
.equ WTDAT,     0x1d30004
.equ WTCNT,     0x1d30008

/* Controlador de IIC */
.equ IICCON,    0x1d60000
.equ IICSTAT,   0x1d60004
.equ IICADD,    0x1d60008
.equ IICDS,     0x1d6000c

/* Controlador de IIS */
.equ IISCON,    0x1d18000
.equ IISMOD,    0x1d18004
.equ IISPSR,    0x1d18008
.equ IISFCON,   0x1d1800c
.equ IISFIF,    0x1d18010

/* Controlador de SIO */
.equ SIOCON,    0x1d14000
.equ SIODAT,    0x1d14004
.equ SBRDR,     0x1d14008
.equ IVTCNT,    0x1d1400c
.equ DCNTZ,     0x1d14010

/*-------------------------------------------------------------------
**
**  Fichero:
**    s3cev40.asm  24/4/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Creación de nemotécnicos para acceso a distintos elementos
**    de la placa de prototipado Embest S3CEV40
**
**  Notas de diseño:
**    Las direcciones usadas asumen un alineamiento little-endian
**
**-----------------------------------------------------------------*/

/* Mapa de memoria */

.equ ROM_START_ADDRESS, 0x00000000
.equ ROM_END_ADDRESS,   0x001fffff

.equ DEVICES_START,     0x01c00000
.equ DEVICES_END,       0x0bffffff

.equ RAM_START_ADDRESS, 0x0c000000
.equ RAM_END_ADDRESS,   0x0c7fffff

.equ SP_START_ADDRESS,  0x10000000
.equ SP_END_ADDRESS,    0x100001ff

.equ CACHE_LRU_START,   0x10004000
.equ CACHE_LRU_END,     0x100047ff

/* Direcciones de las bases de las pilas del sistema */

.equ USRSTACK,      0xc7ff000
.equ SVCSTACK,      0xc7ff100
.equ UNDSTACK,      0xc7ff200
.equ ABTSTACK,      0xc7ff300
.equ IRQSTACK,      0xc7ff400
.equ FIQSTACK,      0xc7ff500

/* Tabla virtual de vectores de excepción */

.equ pISR_RESET,    0xc7fff00
.equ pISR_UNDEF,    0xc7fff04
.equ pISR_SWI,      0xc7fff08
.equ pISR_PABORT,   0xc7fff0c
.equ pISR_DABORT,   0xc7fff10
/* Reservado */
.equ pISR_IRQ,      0xc7fff18
.equ pISR_FIQ,      0xc7fff1c
.equ pISR_ADC,      0xc7fff20
.equ pISR_RTC,      0xc7fff24
.equ pISR_UTXD1,    0xc7fff28
.equ pISR_UTXD0,    0xc7fff2c
.equ pISR_SIO,      0xc7fff30
.equ pISR_IIC,      0xc7fff34
.equ pISR_URXD1,    0xc7fff38
.equ pISR_URXD0,    0xc7fff3c
.equ pISR_TIMER5,   0xc7fff40
.equ pISR_TIMER4,   0xc7fff44
.equ pISR_TIMER3,   0xc7fff48
.equ pISR_TIMER2,   0xc7fff4c
.equ pISR_TIMER1,   0xc7fff50
.equ pISR_TIMER0,   0xc7fff54
.equ pISR_UERR01,   0xc7fff58
.equ pISR_WDT,      0xc7fff5c
.equ pISR_BDMA1,    0xc7fff60
.equ pISR_BDMA0,    0xc7fff64
.equ pISR_ZDMA1,    0xc7fff68
.equ pISR_ZDMA0,    0xc7fff6c
.equ pISR_TICK,     0xc7fff70
.equ pISR_PB,       0xc7fff74 /* EINT4567 */
.equ pISR_ETHERNET, 0xc7fff78 /* EINT3    */
.equ pISR_TS,       0xc7fff7c /* EINT2    */
.equ pISR_KEYPAD,   0xc7fff80 /* EINT1    */
.equ pISR_USB,      0xc7fff84 /* EINT0    */

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
.equ BIT_PB,        (1<<21) /* EINT4567 */
.equ BIT_ETHERNET,  (1<<22) /* EINT3    */
.equ BIT_TS,        (1<<23) /* EINT2    */
.equ BIT_KEYPAD,    (1<<24) /* EINT1    */
.equ BIT_USB,       (1<<25) /* EINT0    */

/* Bits de EXTINTPND */
.equ BIT_RIGHTPB,   (1<<3)
.equ BIT_LEFTPB,    (1<<2)
.equ BIT_IDE_INTRQ, (1<<1)
.equ BIT_IDE_DMACK, (1<<0)


  /* Referencias externas */

  .extern OSIntNesting   /* INT8U OSIntNesting        -- nivel de anidación de interrupciones                  */
  .extern OSTCBCur       /* OS_TCB *OSTCBCur          -- puntero al TCB de la tarea en ejecución (a suspender) */
  .extern OSIntEnter     /* void OSIntEnter( void )   -- función del kernel                                    */
  .extern OSIntExit      /* void OSIntExit( void )    -- función del kernel                                    */

  /* Modos de operación */

  .equ IRQMODE, (0x12)
  .equ SVCMODE, (0x13)

  /* Máscara para desabilitar IRQ y FIQ */

  .equ NOINT,   (0xc0)

/*
**
** Macro para el tratamiento de una interrupción genérica
**
** IMPORTANTE: La rutina comienza su ejecución en modo IRQ (con SP, LR y CPSR propios)
**
*/

.macro OS_CPU_ISR_WRAPPER isr

  stmfd sp!, {r1-r3}           /* Apila los registros de trabajo en la pila IRQ  */

  mov r1, sp                   /* Copia en R1 el puntero de la pila IRQ          */
  add sp, sp, #12              /* Ajusta el puntero del la pila IRQ a la posición que tenía antes de la interrupción (suma 12 porque ha apilado 3 registros) */
  sub r2, lr, #4               /* Copia en R2, previo ajuste, la dirección de retorno a la tarea actual (tras IRQ el PC está en <dir_actual>+4) */
  mrs r3, spsr                 /* Copia en R3 el SPSR (i.e. el CPSR de la tarea actual interrumpida) */

  msr cpsr_c, #(NOINT|SVCMODE) /* Cambia a modo SVC */

                               /* (1) Guarda en la pila SVC el contexto de la tarea actual              */
  stmfd sp!, {r2}              /*     - Apila la dirección de retorno a la tarea actual (previamente copiado en R2) */
  stmfd sp!, {lr}              /*     - Apila el LR de la tarea actual                                  */
  stmfd sp!, {r4-r12}          /*     - Apila los registros R4-R12 de la tarea actual                   */
  ldmfd r1!, {r4-r6}           /*     - Copia en R4-R6 los registros R1-R3 de la tarea actual (previamente almacenados en la pila IRQ) */
  stmfd sp!, {r4-r6}           /*     - Apila los registros R1-R3 de la tarea actual (anteriormente copiados en R4-R6 */
  stmfd sp!, {r0}              /*     - Apila el registro R0 de la tarea actual                         */
  stmfd sp!, {r3}              /*     - Apila el CPSR de la tarea actual (previamente copiado en R3)    */

  bl  OSIntEnter               /* (2) Llama a la función OSIntEnter()          */

  ldr  r0, =OSIntNesting        /* (3) Si OSIntNesting == 1 ...                 */
  ldrb r0, [r0]
  cmp  r0, #1
  bne  label\isr
  ldr  r0, =OSTCBCur            /*     ... OSTCBCur->OSTCBStkPtr = SP           */
  ldr  r1, [r0]
  str  sp, [r1]

label\isr:

  msr cpsr_c, #(NOINT|IRQMODE) /* Cambia a modo IRQ (para que la pila IRQ sea usada para tratar la interrupción */

  bl  \isr                     /* (6) Llama a la función pasada por argumento a la macro esta función deberá:  */
                               /*     - (4) Borrar interrupción pendiente                                      */
                               /*     - (5) Opcionalmente habilitar interrupciones                             */

  msr cpsr_c, #(NOINT|SVCMODE) /* Cambia a modo SVC                            */

  bl  OSIntExit                /* (6) Llama a la función OSIntExit()           */

                               /* Si ha vuelto de OSintExit() porque no hay una tarea preparada con mayor prioridad */
  ldmfd sp!,  {r4}             /* (7) Restaura de la pila el contexto de la tarea actual y ...                      */
  msr   spsr, r4
  ldmfd sp!,  {r0-r12,lr,pc}^  /* (9) ... retorna a dicha tarea                */


.endm



  /* Referencias externas */

  .extern isr_pb         /* void isr_pb( void ) -- RTI por presión de un pushbutton */
  
  /* Funciones definidas en este módudo */

  .global OS_CPU_isr_pb       /* void OS_CPU_isr_pb( void ) */
  
.section .text

/*
** void OS_CPU_isr_pb( void )
**
** Wrapper + isr_pb
**
*/

OS_CPU_isr_pb:
  OS_CPU_ISR_WRAPPER isr_pb

