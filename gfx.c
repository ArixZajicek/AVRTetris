
#include "gfx.h"
#include "lcd.h"
#include "avr.h"

const unsigned char NUMFONT[][5] = {
	{ 0b01111110, 0b10000001, 0b10011001, 0b10000001, 0b01111110 },
	{ 0b10000010, 0b10000001, 0b11111111, 0b10000000, 0b10000000 },
	{ 0b10000010, 0b11000001, 0b10100001, 0b10010001, 0b10001110 },
	{ 0b01000010, 0b10000001, 0b10000001, 0b10001001, 0b01110110 },
	{ 0b00100000, 0b00111000, 0b00100110, 0b11111111, 0b00100000 },
	{ 0b01001111, 0b10000101, 0b10000101, 0b10001001, 0b01110001 },
	{ 0b01111110, 0b10001001, 0b10001001, 0b10001001, 0b01110010 },
	{ 0b00000001, 0b00000001, 0b11111001, 0b00000101, 0b00000011 },
	{ 0b01110110, 0b10001001, 0b10001001, 0b10001001, 0b01110110 },
	{ 0b01001110, 0b10010001, 0b10010001, 0b10010001, 0b01111110 }
};

unsigned char get_bg(unsigned char x) {
	// Basic brick pattern
	unsigned char fill = 0xEE;
	if ((x+1) % 8 == 0) fill &= 0xF0;
	if ((x+5) % 8 == 0) fill &= 0x0F;
	return fill;
}

void gfx_fill_bg() {
	for (int x = 0; x <= 63; x++) {
		lcd_pos(x, 0);
		for (int y = 0; y <= 16; y++) {
			lcd_put(get_bg(x));
		}
	}
}


void gfx_draw_window(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
	unsigned char left = x1, right = x2;
	if (left > 0) left--;
	if (right < 63) right++;
	
	// Central columns (yes, all at once!)
	for (unsigned char i = left; i <= right; i++) {
		
		if (y1 > 0) {
			lcd_pos(i, (y1 - 1) / 2);
			if (y1 % 2 == 0) lcd_get();
			lcd_put((get_bg(i) & 0xFC) | 0x02);
		} else {
			lcd_pos(i, 0);
		}
		
		for (unsigned char j = y1; j <= y2; j++) {
			lcd_put(0x00);
		}
		
		if (y2 < 15) {
			lcd_put((get_bg(i) & 0x3F) | 0x40);
			if (y2 % 2 == 0) lcd_get();
		}
	}
	
	// Left and right edges
	unsigned char i = x1;
	if (x1 > 1) i = x1 - 2;
	else i = x2 + 2;
	for(; i <= 63 && i <= x2 + 2; i += (x2 - x1 + 4)) {
		if (y1 > 0) {
			lcd_pos(i, (y1 - 1) / 2);
			if (y1 % 2 == 0) lcd_get();
			lcd_put(get_bg(i) | 0x03);
		} else {
			lcd_pos(i, 0);
		}
		
		for (int j = y1; j <= y2; j++) {
			lcd_put(0xFF);
		}
		
		if (y2 < 15) {
			lcd_put(get_bg(i) | 0xC0);
			if (y2 % 2 == 0) lcd_get();
		}
	}
}

void gfx_draw_digit(unsigned char x, unsigned char y, unsigned char digit) {
	for (int i = 0; i < 5; i++) {
		lcd_pos(x + i, y / 2);
		if (y % 2 == 1) lcd_get();
		lcd_put(NUMFONT[digit][i]);
		if (y % 2 == 0) lcd_get();
	}
}

void gfx_draw_number(unsigned char x, unsigned char y, unsigned int num) {
	unsigned char drawn = 0;
	unsigned int div = 10000;
	while (div >= 1) {
		if (num / div > 0 || drawn > 0 || div == 1) {
			gfx_draw_digit(x + 6 * drawn, y, num / div);
			drawn++;
		}
		num %= div;
		div = div / 10;
	}
}

void gfx_draw_pixels(unsigned char x, unsigned char y, unsigned char data[], unsigned char length) {
	for (int i = 0; i < length; i++) {
		lcd_pos(x + i, y / 2);
		if (y % 2 == 1) lcd_get();
		lcd_put(data[i]);
		if (y % 2 == 0) lcd_get();
	}
}