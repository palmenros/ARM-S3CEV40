#include "uart.h"
#include "44b.h"

void uart0_init( void )
{
	//*Enable FIFO, Rx normal, Tx normal
	//UFCON0[0] = 1
	//UFCON0[1] = 0  Rx FIFO normal (no reset)
	//UFCON0[2] = 0  Rx FIFO normal (no reset)
    rUFCON0 = 0x1;

    //Disable Auto Flow Control and disable Request to Send
    //UMCON0[4] = 0
    rUMCON0 = 0x0;

    //* Formato de la trama:
    //ULCON0[6]   = 0  normal (no infrarrojos)
    //ULCON0[5:3] = 0  sin paridad
    //ULCON0[2]   = 0  1 bit de stop
    //ULCON0[1:0] = 3  8 bits de datos
    rULCON0 = 0x3;

    //* 115200 Baudios
    //UBRDIV0 = 34  64MHz / (115200 × 16) ‐ 1
    rUBRDIV0 = 34;

    //UCON0[1:0] = 1  Rx: polling/interrupt mode
    //UCON0[3:2] = 1  Tx: polling/interrupt mode
    //UCON0[4]   = 0  no break
    //UCON0[5]   = 0  no loopback
    rUCON0 = 0x5;
}

void uart0_putchar( char ch )
{
	//While FIFO is full wait (while UFSTAT0[9] == 1)
    while( rUFSTAT0 & (1 << 9) );
    rUTXH0 = ch;
}

char uart0_getchar( void )
{
	//While FIFO is empty wait (while UFSTAT0[3:0] == 0)
    while( (rUFSTAT0 & 0b1111) == 0 );

    //Read URXH0
    return rURXH0;
}

void uart0_puts( char *s )
{
    while(*s) {
    	uart0_putchar(*s);
    	s++;
    }
}

void uart0_putint( int32 i )
{
	char s[11 + 1];
	int negative = i < 0;
	char* p = s + 11;

	*p = '\0';
	if (negative)
		i *= -1;

	do {
		*--p = '0' + ( i % 10 );
		i /= 10;
	} while(i > 0);

	if(negative) {
		*--p = '-';
	}

	uart0_puts(p);
}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{
	char c = uart0_getchar();

	while(c != '\n') {
		*s = c;
		s++;
		c = uart0_getchar();
	}

	*s = '\0';
}

int32 uart0_getint( void )
{
	char s[11 + 1];
	char* p = s;
	int32 res = 0;

	uart0_gets(s);

	if(s[0] == '\0')
		return 0;

	if(s[0]=='-')
		p++;

	while(*p) {
		res = 10 * res + (*p - '0');
		p++;
	}

	if(s[0] == '-')
		res *= -1;

	return res;
}

uint32 uart0_gethex( void )
{
	char s[8 + 1];
	char* p = s;
	int32 res = 0;

	uart0_gets(s);

	while(*p) {
		res = res << 4;
		char c = *p;
		if(c >= '0' && c <= '9')
			res += c - '0';
		else if(c >= 'a' && c <= 'f')
			res += 10 + c - 'a';
		else if(c >= 'A' && c <= 'F')
			res += 10 + c - 'A';
		p++;
	}

	return res;
}

void uart0_put_rtc_time( rtc_time_t* rtc_time )
{
	uart0_putint( rtc_time->mday );
	uart0_putchar( '/' );
	uart0_putint( rtc_time->mon );
	uart0_putchar( '/' );
	uart0_putint( rtc_time->year );
	uart0_putchar( ' ' );
	uart0_putint( rtc_time->hour );
	uart0_putchar( ':' );
	uart0_putint( rtc_time->min );
	uart0_putchar( ':' );
	uart0_putint( rtc_time->sec );
}

