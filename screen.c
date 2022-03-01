
#include "avr.h"
#include "lcd.h"

const unsigned int WIDTH = 64;
const unsigned int HEIGHT = 128;

// this uses half of the ram-- yikes!
unsigned char* scr_new() {
	unsigned char scr = malloc(WIDTH * HEIGHT / 8);
}

void scr_set_pixel(unsigned char* scr, unsigned char x, unsigned char y, unsigned char val) {
	
}

void scr_draw_rect(unsigned char* scr, unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char val) {
	
}

void scr_fill_rect(unsigned char* scr, unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char val) {
	
}
