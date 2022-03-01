#include <xc.h>
#include "avr.h"
#include "lcd.h"
#include "gfx.h"
#include "tetris.h"
#include "control.h"

int main(void)
{	
	lcd_init();
	
	// Timer triggers every 250ms
	// TCNT1 = 34286;
	TCNT1 = 65000;
	TCCR1A = 0x00;
	TCCR1B = (1<<CS11) | (1<<CS10);
	TIMSK = 1 << TOIE1;
	sei();
	
	unsigned char controller = 0, last_controller = 0;
	unsigned char state = 0;
    while(1)
    {
		// Controller
		last_controller = controller;
		controller = control_get();
		unsigned char controller_new = controller ^ last_controller;
		controller_new &= controller;
		
		switch (state) {
		case 0: // Intro
			state = 10;
			break;
		case 10: // Options menu
			gfx_fill_bg(); // Bricks
			if (controller_new & Start) state = 19;
			break;
		case 19: // Set up screen
			gfx_draw_window(20, 2, 59, 11); // Main tetris window
			gfx_draw_window(4, 5, 11, 11);  // Next few pieces
			gfx_draw_window(4, 2, 11, 3);   // Held piece
			gfx_draw_window(4, 14, 59, 14); // Linecount
			t_init(0);
			state = 20;
			break;
		case 20: // Gameplay
			t_tick(&state, controller_new);
			t_draw_board();
			break;
		case 21: // Clear window for pause (so people can't spam pause and cheat at faster speeds)
			gfx_draw_window(20, 2, 59, 11); // Main tetris window
			state = 22;
			break;
		case 22: // Wait in pause mode
			if (controller_new & Start) state = 20;
			break;
		case 30: // Game over
			if (controller_new & Start) state = 19;
			break;
		case 100: // Testing data
			for (int x = 0; x < 64; x++) {
				lcd_pos(x, 0);
				if (x == 32) controller = 0b10101010;
				for (int j = 0; j < 8; j++) {
					lcd_put(((controller & (0x01 << j)) ? 0xFF : 0x00));
					lcd_put(((controller & (0x01 << j)) ? 0xFF : 0x00));
				}
			}
			break;
		}
		avr_wait(25);
	}
}

// Timer 1 overflow interrupt (most important, update the time)
ISR (TIMER1_OVF_vect)
{
	TCNT1 = 65000;
	t_random();
}