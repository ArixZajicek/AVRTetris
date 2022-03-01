/* The purpose of this file is entirely to provide easier graphics
 * abilities to the LCD instead of having to draw a lot of stuff manually.*/

#ifndef _GFX_H_
#define _GFX_H_

void gfx_fill_bg();
void gfx_draw_window(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void gfx_draw_number(unsigned char x, unsigned char y, unsigned int num);
void gfx_draw_pixels(unsigned char x, unsigned char y, unsigned char *data, unsigned char length);

#endif