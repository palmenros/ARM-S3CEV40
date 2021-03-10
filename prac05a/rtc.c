#include "rtc.h"
#include "44b.h"
#include "intcontroller.h"

void isr_TICK_dummy(void)  __attribute__ ((interrupt ("IRQ")));
void isr_ALARM_dummy(void) __attribute__ ((interrupt ("IRQ")));

void isr_TICK_dummy(void){
	ic_cleanflag(INT_TICK);
}

void isr_ALARM_dummy(void){
	ic_cleanflag(INT_RTC);
}

void rtc_init( void )
{
	//Disable Tick interrupt
    rTICINT   = 0x0;

    //Disable alarm
    rRTCALM  = 0x0;

    //Disable Rounding
    rRTCRST  = 0x0;


    // RTCCON[2] = 0 merged BCD registers: única opción posible
    // RTCCON[1] = 0 Fuente de reloj primaria: única opción posible: XTAL
    // RTCCON[0] = 1 Enabled register read/write
    rRTCCON  = 0x1;

    //Set to 00:00:00 1/1/2021
    rBCDYEAR = 0x2 << 4 | 0x1;
    rBCDMON  = 0x1;
    rBCDDAY  = 0x1;
    rBCDDATE = 0x4;
    rBCDHOUR = 0x0;
    rBCDMIN  = 0x0;
    rBCDSEC  = 0x0;

    //Init to 0
    rALMYEAR = 0;
    rALMMON  = 0;
    rALMDAY  = 0;
    rALMHOUR = 0;
    rALMMIN  = 0;
    rALMSEC  = 0;

    rRTCCON &= ~1;
}

void rtc_puttime( rtc_time_t *rtc_time )
{
	//Set RTCEN=RTCCON[0] = 1
    rRTCCON |= 1;

    rBCDYEAR = ((rtc_time->year / 10) % 10) << 4 | (rtc_time->year % 10);
    rBCDMON  = ((rtc_time->mon / 10) % 10) << 4 | (rtc_time->mon % 10);
    rBCDDAY  = ((rtc_time->mday / 10) % 10) << 4 | (rtc_time->mday % 10);
    rBCDDATE = rtc_time->wday;
    rBCDHOUR = ((rtc_time->hour / 10) % 10) << 4 | (rtc_time->hour % 10);
    rBCDMIN  = ((rtc_time->min / 10) % 10) << 4 | (rtc_time->min % 10);
    rBCDSEC  = ((rtc_time->sec / 10) % 10) << 4 | (rtc_time->sec % 10);

	//Set RTCEN=RTCCON[0] = 0
    rRTCCON &= ~1;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
	//Set RTCEN=RTCCON[0] = 1
    rRTCCON |= 1;

    rtc_time->year = (rBCDYEAR & 0b1111) + 10 * (rBCDYEAR >> 4);
    rtc_time->mon  = (rBCDMON & 0b1111) + 10 * (rBCDMON >> 4);
    rtc_time->mday = (rBCDDAY & 0b1111) + 10 * (rBCDDAY >> 4);
    rtc_time->wday = rBCDDATE;
    rtc_time->hour = (rBCDHOUR & 0b1111) + 10 * (rBCDHOUR >> 4);
    rtc_time->min  = (rBCDMIN & 0b1111) + 10 * (rBCDMIN >> 4);
    rtc_time->sec  = (rBCDSEC & 0b1111) + 10 * (rBCDSEC >> 4);
    if( ! rtc_time->sec ){
    	//Reread, a second may have elapsed so date is invalid
        rtc_time->year = (rBCDYEAR & 0b1111) + 10 * (rBCDYEAR >> 4);
        rtc_time->mon  = (rBCDMON & 0b1111) + 10 * (rBCDMON >> 4);
        rtc_time->mday = (rBCDDAY & 0b1111) + 10 * (rBCDDAY >> 4);
        rtc_time->wday = rBCDDATE;
        rtc_time->hour = (rBCDHOUR & 0b1111) + 10 * (rBCDHOUR >> 4);
        rtc_time->min  = (rBCDMIN & 0b1111) + 10 * (rBCDMIN >> 4);
        rtc_time->sec  = (rBCDSEC & 0b1111) + 10 * (rBCDSEC >> 4);
    }

	//Set RTCEN=RTCCON[0] = 0
    rRTCCON &= ~1;
}

void rtc_set_alarm( void(*isr)(void), rtc_time_t* rtc_time ) {


    //Set alarm date
    rALMYEAR = ((rtc_time->year / 10) % 10) << 4 | (rtc_time->year % 10);
    rALMMON  = ((rtc_time->mon / 10) % 10) << 4 | (rtc_time->mon % 10);
    rALMDAY  = ((rtc_time->mday / 10) % 10) << 4 | (rtc_time->mday % 10);
    rALMHOUR = ((rtc_time->hour / 10) % 10) << 4 | (rtc_time->hour % 10);
    rALMMIN  = ((rtc_time->min / 10) % 10) << 4 | (rtc_time->min % 10);
    rALMSEC  = ((rtc_time->sec / 10) % 10) << 4 | (rtc_time->sec % 10);

	//Enable alarm (bits [7:0] = 1)
    rRTCALM  = 0x7F;

    ic_conf_line(INT_RTC, IRQ);
    ic_cleanflag(INT_RTC);
    ic_enable(INT_RTC);

    pISR_RTC = (unsigned) isr;
}

void rtc_clear_alarm() {
	ic_disable(INT_RTC);
	pISR_RTC = (unsigned) isr_ALARM_dummy;
    rRTCALM  = 0;
}

// Sets ISR to RTC ticks (tick_count)
void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = (unsigned)isr;

    ic_conf_line(INT_TICK, IRQ);
    ic_cleanflag(INT_TICK);
    ic_enable(INT_TICK);

    rTICINT = 1 << 7 | (tick_count & 0b1111111);
}

// Removes RTC ISR
void rtc_close( void )
{
    rTICINT = 0;
    ic_disable(INT_TICK);
    pISR_TICK = (unsigned)isr_TICK_dummy;
}

