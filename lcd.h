#ifndef _LCD_H_
#define _LCD_H_

/* Initialize the LCD into graphical mode */
void lcd_init(void);

/* This is now a graphical clear, not character clear. */
void lcd_clr(void);

/* This is now graphical. X is from 0-63, Y is from 0-7 (because Y is which 16px section counting up from the bottom) */
void lcd_pos(unsigned char x, unsigned char y);

/* Put a line at a location. */
void lcd_put(unsigned char c);

/* The LCD requires two bytes to be accessed from screen ram at a time. This function reads a byte from screen
 * ram in case we are only interested in modifying one of the two bytes, that way we don't have to rewrite a byte and
 * it satisfies the requirement of accessing two bytes. Of course, it also returns that byte in screen ram, but the
 * former usage is extremely helpful. */
unsigned char lcd_get();

#endif /* _LCD_H_ */