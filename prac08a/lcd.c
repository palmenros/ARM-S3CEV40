#include "44b.h"
#include "lcd.h"

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;

void lcd_init( void )
{
	//Initialize dithering patterns to recommended patterns
	rDITHMODE = 0x12210;
	rDP1_2 = 0xA5A5;
	rDP4_7 = 0xBA5DA65;
	rDP3_5 = 0xA5A5F;
	rDP2_3 = 0xD6B;
	rDP5_7 = 0xEB7B5ED;
	rDP3_4 = 0x7DBE;
	rDP4_5 = 0x7EBDF;
	rDP6_7 = 0x7FDFBFE;

	//Zero, black and white LCD
    rREDLUT   = 0;
    rGREENLUT = 0;
    rBLUELUT  = 0;

    //LCDCON1[0]     = 0   Video Output Initially Off
    //LCDCON1[1]     = 0   no invierte señales de vídeo
    //LCDCON1[2]     = 0   no invierte la señal VFRAME
    //LCDCON1[3]     = 0   no invierte la señal VLINE
    //LCDCON1[4]     = 0   las señales de vídeo se muestrean a flanco de bajada
    //LCDCON1[9:8]   = 0   WDLY = 4 ciclos (minimum blanking)
    //LCDCON1[11:10] = 0   WLH = 4 ciclos  (minimum blanking)
    //LCDCON1[21:12] = 28  CLKVAL = 28 => tFRAME = 59,4 Hz
    rLCDCON1  = 0x1C020;

    //LCDCON2[31:21] =  0 LINEBLANK = 0 ciclos (minimum horizontal blanking
    //LCDCON2[20:10] = 79   HOZVAL = (320 / 4) – 1 (Horizontal resolution)
    //LCDCON2[9:0]   = 239  LINEVAL = 240 – 1 (Vertical resolution)
    rLCDCON2  = 0x13CEF;

    //LCDCON3[0] = 0  Disable self refresh
    rLCDCON3  = 0;

    //Start of lcd_buffer
    rLCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);

    //End of lcd_buffer
    rLCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;

    //No virtual screen
    //LCDSADDR3[19:9] = 0 OFFSIZE = 0
    //LCDSADDR3[8:0] = 80 PAGEWIDTH = HOZVAL + 1 = 320 / 4
    rLCDSADDR3 = 0x50;

    lcd_off();
}

void lcd_on( void )
{
    rLCDCON1 |= 1;
	state = 1;
}

void lcd_off( void )
{
    rLCDCON1 &= ~1;
	state = 0;
}

uint8 lcd_status( void )
{
	return state;
}

void lcd_clear( void )
{
	uint16 i = 0;
    while(i < LCD_BUFFER_SIZE) {
    	lcd_buffer[i] = 0x0;
    	i++;
    }
}

void lcd_clear_color( uint8 color )
{
	uint16 i = 0;
	uint8 c = color & 0xf;

    while(i < LCD_BUFFER_SIZE) {
    	lcd_buffer[i] = (c << 4) | c;
    	i++;
    }
}


void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;

    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
	uint8 byte, bit;
	uint16 i = x/2 + y*(LCD_WIDTH/2);

	bit = (1-x%2)*4;
	byte = lcd_buffer[i];

	return (byte >> bit ) & 0xf;
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{
	int x_i, y_i;
	for(x_i = xleft; x_i <= xright; ++x_i)
		for(y_i = y; y_i < y + width; ++y_i)
			lcd_putpixel(x_i, y_i, color);
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{
	int x_i, y_i;
	for(y_i = yup; y_i <= ydown; ++y_i)
		for(x_i = x; x_i < x + width; ++x_i)
			lcd_putpixel(x_i, y_i, color);
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
	lcd_draw_hline(xleft, xright, yup, color, width);
	lcd_draw_vline(yup, ydown, xleft, color, width);
	lcd_draw_vline(yup, ydown, xright - width + 1, color, width);
	lcd_draw_hline(xleft, xright, ydown - width + 1, color, width);
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	while(*s) {
		lcd_putchar(x, y, color, *s);

		x += 8;
		if(x + 8 >= LCD_WIDTH) {
			x = 0;
			y += 16;
		}
		s++;
	}
}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;

    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}

