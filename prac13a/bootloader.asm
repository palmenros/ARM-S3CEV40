/*-----------------------------------------------------------------
**
**  Fichero:
**    bootloader.asm  26/3/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Bootloader elemental genérico para aplicaciones desarrolladas
**    para la placa de prototipado Embest S3CEV40
**
**  Notas de diseño:
**    - Asume ser una unica imagen plana ubicada al inicio de la ROM
**    - Replica un mapa de memoria similar al existente en proyectos
**      volcados en RAM (pilas y tabla virtual de vectores de excepción)
**    - Permite el portado directo de proyectos volcados en RAM
**    - Asume que las RTI están a menos de 32MB de distancia, en otro
**      caso reemplazar "B etq" por "LDR PC,=etq"
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

/*
** Modos de operación
*/
.equ USRMODE, 0x10
.equ FIQMODE, 0x11
.equ IRQMODE, 0x12
.equ SVCMODE, 0x13
.equ ABTMODE, 0x17
.equ UNDMODE, 0x1b
.equ SYSMODE, 0x1f

/*
** Macro para la generación de un prologo para una RTI que, dado un puntero a su
** comienzo (entrada de la tabla virtual de vectores de excepcon), salte a la RTI
*/
.macro HANDLER pISR
  sub   sp,  sp, #4    /* deja un hueco en pila para almacenar la dirección de la RTI */
  stmfd sp!, {r0}      /* apila r0 (registro de trabajo) */
  ldr   r0,  =\pISR    /* carga en r0 la dirección de entrada de la tabla virtual de vectores indicada */
  ldr   r0,  [r0]      /* accediendo a dicha entrada carga en r0 la direccion de la RTI */
  str   r0,  [sp,#4]   /* almacena en pila la direccion de la RTI */
  ldmfd sp!, {r0, pc}  /* restaura el r0 y salta a la RTI */
.endm

.extern _image_start
.extern _image_end

.extern main

.section .text

/*
** Tabla de vectores de excepción: saltos a los prologos de cada RTE
*/
  b bootloader         /* salta al comienzo del programa de arranque */
  b handlerUndef
  b handlerSWI
  b handlerPabort
  b handlerDabort
  nop                  /* Reservado */
  b handlerIRQ
  b handlerFIQ

/*
** Tabla de vectores de IRQ: saltos a los prologos de cada RTI
*/
  b handlerUSB         /* EINT0 */
  b handlerKEYPAD      /* EINT1 */
  b handlerTS          /* EINT2 */
  b handlerETHERNET    /* EINT3 */
  b handlerPB          /* EINT4567 */
  b handlerTICK
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerZDMA0
  b handlerZDMA1
  b handlerBDMA0
  b handlerBDMA1
  b handlerWDT
  b handlerUERR01
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerTIMER0
  b handlerTIMER1
  b handlerTIMER2
  b handlerTIMER3
  b handlerTIMER4
  b handlerTIMER5
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerURXD0
  b handlerURXD1
  b handlerIIC
  b handlerSIO
  b handlerUTXD0
  b handlerUTXD1
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerRTC
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerADC

/*
** Prologos de RTE
*/
  handlerUndef:    HANDLER pISR_UNDEF
  handlerSWI:      HANDLER pISR_SWI
  handlerPabort:   HANDLER pISR_PABORT
  handlerDabort:   HANDLER pISR_DABORT
  handlerIRQ:      HANDLER pISR_IRQ
  handlerFIQ:      HANDLER pISR_FIQ

/*
** Prologos de RTI
*/
  handlerUSB:      HANDLER pISR_USB      /* EINT0 */
  handlerKEYPAD:   HANDLER pISR_KEYPAD   /* EINT1 */
  handlerTS:       HANDLER pISR_TS       /* EINT2 */
  handlerETHERNET: HANDLER pISR_ETHERNET /* EINT3 */
  handlerPB:       HANDLER pISR_PB       /* EINT4567 */
  handlerTICK:     HANDLER pISR_TICK
  handlerZDMA0:    HANDLER pISR_ZDMA0
  handlerZDMA1:    HANDLER pISR_ZDMA1
  handlerBDMA0:    HANDLER pISR_BDMA0
  handlerBDMA1:    HANDLER pISR_BDMA1
  handlerWDT:      HANDLER pISR_WDT
  handlerUERR01:   HANDLER pISR_UERR01
  handlerTIMER0:   HANDLER pISR_TIMER0
  handlerTIMER1:   HANDLER pISR_TIMER1
  handlerTIMER2:   HANDLER pISR_TIMER2
  handlerTIMER3:   HANDLER pISR_TIMER3
  handlerTIMER4:   HANDLER pISR_TIMER4
  handlerTIMER5:   HANDLER pISR_TIMER5
  handlerURXD0:    HANDLER pISR_URXD0
  handlerURXD1:    HANDLER pISR_URXD1
  handlerIIC:      HANDLER pISR_IIC
  handlerSIO:      HANDLER pISR_SIO
  handlerUTXD0:    HANDLER pISR_UTXD0
  handlerUTXD1:    HANDLER pISR_UTXD1
  handlerRTC:      HANDLER pISR_RTC
  handlerADC:      HANDLER pISR_ADC

/*
** Comienzo del bootloader:
**   La CPU está en estado ARM y en modo SVC
**   IRQ y FIQ están deshabilitadas
**   Todos los registros de los controladores de dispositivo están inicializados al valor indicado por el fabricante
**   El bootloader esta ejecutandose desde ROM a 8 MHz
*/
bootloader:

  /* Deshabilita el watchdog */
  ldr r0, =WTCON
  ldr r1, =0x0
  str r1, [r0]

  /* Configuracion del gestor de reloj */
  ldr r0, =LOCKTIME   /*  estabilización del PLL = 512 us */
  ldr r1, =0xfff
  str r1, [r0]
  ldr r0, =PLLCON     /* MCLK = 64 MHz */
  ldr r1, =0x38021
  str r1, [r0]
  ldr r0, =CLKSLOW    /* MCLK_SLOW = 500 KHz */
  ldr r1, =0x8
  str r1, [r0]
  ldr r0, =CLKCON     /* modo NORMAL y reloj distribuido a todos los controladores */
  ldr r1, =0x7ff8
  str r1, [r0]

  /* Configuracion del gestor de memoria */
  ldr   r0, =memconf
  ldr   r1, =_image_start
  sub   r0, r0, r1         /* corrige la diferencia existente entre la dirección en ROM y la direccion indicada en el linker script */
  ldmia r0, {r1-r13}       /* El controlador se programa usando ldmia/stmia según lo indicado por el fabricante */
  ldr   r0, =BWSCON
  stmia r0, {r1-r13}

  /* Copia en RAM la imagen del programa cargado en ROM a partir de la dirección indicada en el linker script */
  ldr   r0, =ROM_START_ADDRESS
  ldr   r1, =_image_start
  ldr   r2, =_image_end
remap_loop:
  cmp   r1, r2
  ldrcc r3, [r0], #4    /* Lee una palabra de la ROM (post-incrementando indice)    */
  strcc r3, [r1], #4    /* Escribe la palabra en la RAM (post-incrementando indice) */
  bcc   remap_loop

  ldr pc, =ram_exec     /* A partir de este punto el bootoloader pasa a ejecutarse desde RAM   */
                        /* La dirección absoluta a la que se corresponde la etiqueta apunta a RAM */

ram_exec:

  /* Inicializacion de los SP correspondiente a los modos de ejecución privilegiados */
  mrs r0, cpsr
  bic r0, r0, #0x1f

  orr r1, r0, #UNDMODE  /* desde modo SVC cambia a modo UND e inicializa el SP_und */
  msr cpsr_c, r1
  ldr sp, =UNDSTACK

  orr r1, r0, #ABTMODE  /* desde modo UND cambia a modo ABT e inicializa el SP_abt */
  msr cpsr_c, r1
  ldr sp, =ABTSTACK

  orr r1, r0, #IRQMODE  /* desde modo ABT cambia a modo IRQ e inicializa el SP_abt */
  msr cpsr_c, r1
  ldr sp, =IRQSTACK

  orr r1, r0, #FIQMODE  /* desde modo IRQ cambia a modo FIQ e inicializa el SP_fiq */
  msr cpsr_c, r1
  ldr sp, =FIQSTACK

  orr r1, r0, #SVCMODE  /* desde modo FIQ cambia a modo SVC e inicializa el SP_svc */
  msr cpsr_c, r1
  ldr sp, =SVCSTACK

  /* Habilita las líneas IRQ/FIQ permaneciendo en modo SVC */
  mrs  r0, cpsr
  bic  r0, r0, #0xc0
  msr  cpsr_c, r0

  /* El bootloader cede el control al programa principal en C */
  bl  main
  b   .                 /* si retorna de main, ejecuta un bucle infinito */

/*
** Constantes de configuracion del controlador de memoria
*/
memconf:
  .word 0x01000000 /* BWSCON   */
  .word 0x00000400 /* BANKCON0 */
  .word 0x00007ffc /* BANKCON1 */
  .word 0x00000000 /* BANKCON2 */
  .word 0x00007ffc /* BANKCON3 */
  .word 0x00000000 /* BANKCON4 */
  .word 0x00000000 /* BANKCON5 */
  .word 0x00018000 /* BANKCON6 */
  .word 0x00000000 /* BANKCON7 */
  .word 0x0086041b /* REFRESH  */
  .word 0x00000016 /* BANKSIZE */
  .word 0x00000020 /* MRSR6    */
  .word 0x00000000 /* MRSR7    */

.ltorg

.end
