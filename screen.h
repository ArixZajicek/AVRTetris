#ifndef _SCREEN_H_
#define _SCREEN_H_

unsigned char* scr_new();

void scr_set_pixel(unsigned char* scr, unsigned char x, unsigned char y, unsigned char val);

void scr_draw_rect(unsigned char* scr, unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char val);

void scr_fill_rect(unsigned char* scr, unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char val);

#endif /* _SCREEN_H_ */