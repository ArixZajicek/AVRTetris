/**
 * lcd.c
 * Copyright (C) 2001-2020, Tony Givargis
 */

#include "avr.h"
#include "lcd.h"

#define DDR    DDRB
#define PORT   PORTB
#define RS_PIN 0
#define RW_PIN 1
#define EN_PIN 2

static void set_data(unsigned char x) {
	PORTD = x;
	DDRD = 0xff;
}

static unsigned char get_data(void) {
	DDRD = 0x00;
	return PIND;
}

static void sleep_700ns(void) {
	NOP();
	NOP();
	NOP();
}

static unsigned char input(unsigned char rs) {
	unsigned char d;

	if (rs) SET_BIT(PORT, RS_PIN);
	else CLR_BIT(PORT, RS_PIN);
	
	SET_BIT(PORT, RW_PIN);
	get_data();
	SET_BIT(PORT, EN_PIN);
	sleep_700ns();
	d = get_data();
	CLR_BIT(PORT, EN_PIN);
	return d;
}

static void output(unsigned char d, unsigned char rs)
{
	if (rs)  SET_BIT(PORT, RS_PIN);
	else  CLR_BIT(PORT, RS_PIN);
	
	CLR_BIT(PORT, RW_PIN);
	set_data(d);
	SET_BIT(PORT, EN_PIN);
	sleep_700ns();
	CLR_BIT(PORT, EN_PIN);
}

static void write(unsigned char c, unsigned char rs)
{
	while (input(0) & 0x80);
	output(c, rs);
}

void lcd_init(void)
{
	SET_BIT(DDR, RS_PIN);
	SET_BIT(DDR, RW_PIN);
	SET_BIT(DDR, EN_PIN);
	avr_wait(16);
	output(0b00110000, 0);
	avr_wait(5);
	output(0b00110000, 0);
	avr_wait(1);
	write(0b00110000, 0); // 8-bit, basic instructions
	write(0b00001100, 0); // Display ON, Cursor OFF and Blink OFF
	write(0b00000110, 0); // Cursor inc, no shift
	write(0b00000001, 0); // Clear display
	
	write(0b00110110, 0); // Switch to extended ins, change graphic display on
	write(0b00110000, 0); // Switch to basic ins
	
	// Clear graphics ram.
	lcd_clr();
}

void lcd_clr() {
	for (int vert = 0; vert < 32; vert++) {
		
		write(0b00110110, 0); // Switch to extended ins
		write(0x80 | (0x3F & vert), 0); // Set vertical address
		write(0x80 | (0x0F & 0), 0); // Set horiz address
		write(0b00110000, 0); // Switch to basic ins
		
		for (int horiz = 0; horiz < 32; horiz++) {
			write(0x00, 1); // Write to GDRAM
		}
	}
}

void lcd_pos(unsigned char x, unsigned char y) {
	if (x >= 32) {
		x -= 32;
		y += 8;
	}
	write(0b00110110, 0); // Switch to extended ins
	write(0x80 | (0x3F & x), 0); // Set vertical address
	write(0x80 | (0x0F & y), 0); // Set horiz address
	write(0b00110000, 0); // Switch to basic ins
}

void lcd_put(unsigned char c) {
	write(c, 1);
}

unsigned char lcd_get(){
	while (input(0) & 0x80);
	return input(1);
}