#include <stdlib.h>
#include "avr.h"
#include "lcd.h"
#include "gfx.h"
#include "control.h"

const unsigned int BLOCKS[] = {
	0,
	0b1111100110011111,		// Square
	0b1111111111111111,		// Line
	0b1111111111011111,		// T
	0b1111110110111111,		// Z
	0b1111101111011111,		// S
	0b1111101110011111,		// L
	0b1111100110111111		// Preschool L
};

const unsigned char PIECE_SHAPES[] = {
	0,
	0b01100110,
	0b00001111,
	0b01110010,
	0b01100011,
	0b00110110,
	0b01110100,
	0b01000111
};

unsigned char gameover = 0;
unsigned char level = 0;
unsigned char lines = 0;
unsigned int queue = 0;
unsigned char hold_delay = 0;
unsigned char hold_piece = 0;
signed char move_delay = 0;
unsigned char active_type = 0;
unsigned char active_pos[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

unsigned char drop_count = 0;
unsigned char has_hit_bottom = 0;
unsigned char gameboard[120];

unsigned char t_get_piece(unsigned char x, unsigned char y) {
	return (gameboard[12 * x + y / 2] >> (4*(y % 2))) & 0x07; 
}

void t_set_piece(unsigned char x, unsigned char y, unsigned char piece) {
	if (y < 22) {
		gameboard[12 * x + y / 2] &= 0xF0 >> (4 * (y % 2));
		gameboard[12 * x + y / 2] |= (piece & 0x0F) << (4 * (y % 2));
	}
}

unsigned char rnum;
unsigned char t_random() {
	return ++rnum;
}

void t_fill_pos(unsigned char a[4][2]) {
	for (unsigned char i = 0; i < 4; i++) {
		active_pos[i][0] = a[i][0];
		active_pos[i][1] = a[i][1];
	}
}

void t_draw_board() {
	// Place the 4 pieces physically onto the board just to have them drawn as regular blocks
	if (active_type > 0) {
		for (int i = 0; i < 4; i++) {
			if (active_pos[i][1] < 20) t_set_piece(active_pos[i][0], active_pos[i][1], active_type);
		}
	}
	
	for (int x = 0; x < 40; x++) {
		lcd_pos(x + 20, 1);
		for (int y = 0; y < 20; y+=2) {
			unsigned char fill = (BLOCKS[t_get_piece(x / 4, y)] >> (4*(x%4))) & 0x000F;
			fill <<= 4;
			fill |= (BLOCKS[t_get_piece(x / 4, y + 1)] >> (4*(x % 4))) & 0x000F;
			
			lcd_put(fill);
		}
	}
	
	// Remove the 4 pieces back off of the board
	if (active_type > 0) {
		for (int i = 0; i < 4; i++) {
			if (active_pos[i][1] < 20) t_set_piece(active_pos[i][0], active_pos[i][1], 0);
		}
	}
}

void t_commit_piece() {
	for (unsigned char j = 0; j < 4; j++) {
		t_set_piece(active_pos[j][0], active_pos[j][1], active_type);
	}
	
	active_type = 0;
	
	unsigned char line_bits = 0;
	unsigned char line_count = 0;
	// Now check to see if we clear any lines
	for (unsigned char i = 0; i < 4; i++) {
		// First, make sure this line has not been checked before
		unsigned char skip = 0;
		for (unsigned char j = 0; j < i; j++) if (active_pos[i][1] == active_pos[j][1]) skip = 1;
		if (skip == 1) continue;
		
		// Looks like we're good, let's check
		unsigned char full_line = 1;
		for (int x = 0; x < 10; x++) {
			if (t_get_piece(x, active_pos[i][1]) == 0) full_line = 0;
		}
		
		if (full_line == 1) {
			line_bits |= (0x01 << i);
			++line_count;	
		}
	}

	
	
	// If we cleared a line, this will have a value. Let's play through the clearing animation.
	if (line_count > 0) {
		for (unsigned char frame = 0; frame < 8; frame++) {
			for (unsigned char line = 0; line < 4; line++) if (line_bits & (0x01 << line)) {
				for (unsigned char x = 0; x < 10; x++) t_set_piece(x, active_pos[line][1], ((1 + frame) % 2) * 2);
			}
			t_draw_board();
			for (unsigned int d = 0; d < 10000; d++) NOP();
		}
		
		// Animation is over, now actually clear the lines. This must be done from highest line to lowest.
		while (line_bits != 0) {
			unsigned char line = 0;
			unsigned char highest_line = 0;
			for (unsigned char i = 0; i < 4; i++) if (line_bits & (0x01 << i)) {
				if (active_pos[i][1] >= highest_line) {
					line = i;
					highest_line = active_pos[i][1];
				}
			}
			
			line_bits &= ~(0x01 << line);
				
			for (unsigned char y = active_pos[line][1]; y < 23; y++) {
				for (unsigned char x = 0; x < 10; x++) {
					t_set_piece(x, y, t_get_piece(x, y + 1));
				}
			}
		}
		for (unsigned char x = 0; x < 10; x++) t_set_piece(x, 23, 0);
		
		lines += line_count;
		
		/*
		for (int i = 0; i < 56; i++) {
			lcd_pos(4 + i, 7);
			lcd_put((lines & (0x80 >> (i / 7))) ? 0xFF : 0x00);
			lcd_get();
		}*/
		gfx_draw_number(4, 14, lines);
		
		// Redraw
		t_draw_board();
	}
}

void draw_piece(unsigned char x, unsigned char y, unsigned char p) {
	unsigned char pad = y & 0x80;
	if (pad) pad = 1;
	y &= 0x7F;
	
	for (int i = 0; i < 8; i++) {
		lcd_pos(x + i, y / 2);
		if (y % 2) lcd_get();
		
		unsigned char fill = 0;
		for (int j = 0; j < 4; j++) {
			fill <<= 4;
			if ((PIECE_SHAPES[p] >> (4 - 4 * (i / 4))) & (0x08 >> j)) fill |= (BLOCKS[p] >> (4*(i%4))) & 0x000F;
			if (j % 2 == (1-pad)) lcd_put(fill);
		}
		
		if (pad) {
			fill <<= 4;
			lcd_put(fill);
		}
		
		if (y % 2 != pad) lcd_get();
	}
}

// Gives the next piece a location on the board
void t_next_piece() {
	if (hold_delay > 0) --hold_delay;
	active_type = queue & 0x0F;
	queue = queue >> 4;
	queue |= (((t_random() % 7) + 1) << 12);
	
	// Place the piece on the board
	switch(active_type) {
		case 1: // Square
			t_fill_pos((unsigned char[4][2]){{4, 19}, {4, 20}, {5, 19}, {5, 20}});
		break;
		case 2:	// Line
			t_fill_pos((unsigned char[4][2]){{3, 19}, {4, 19}, {5, 19}, {6, 19}});
		break;
		case 3:	// T
			t_fill_pos((unsigned char[4][2]){{4, 19}, {5, 19}, {6, 19}, {5, 20}});
		break;
		case 4:	// Z
			t_fill_pos((unsigned char[4][2]){{4, 20}, {5, 20}, {5, 19}, {6, 19}});
		break;
		case 5:	// S
			t_fill_pos((unsigned char[4][2]){{4, 19}, {5, 19}, {5, 20}, {6, 20}});
		break;
		case 6:	// L
			t_fill_pos((unsigned char[4][2]){{4, 19}, {4, 20}, {5, 20}, {6, 20}});
		break;
		case 7:	// Preschool L
			t_fill_pos((unsigned char[4][2]){{6, 19}, {4, 20}, {5, 20}, {6, 20}});
		break;
	}
	
	// Check if the piece already overlaps with another. If so, the game is over.
	unsigned char overlap = 0;
	for (int i = 0; i < 4; i++) {
		if (t_get_piece(active_pos[i][0], active_pos[i][1])) {
			overlap = 1;
			break;
		}
	}
	
	if (overlap) {
		active_type = 0;
		queue = 0;
		gameover = 1;
		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 10; x++) t_set_piece(x, y, 1);
			t_draw_board();
			for (unsigned int d = 0; d < 25000; d++) NOP();
		}
		
		draw_piece(4, 2, 0);
	}
	
	// Draw the new pieces in the side window
	draw_piece(4, 5, (queue >> 8) & 0x0F);
	draw_piece(4, 7 | 0x80, (queue >> 4) & 0x0F);
	draw_piece(4, 10, queue & 0x0F);
}

void t_init(unsigned char starting_level) {
	// Clear the gameboard
	for (unsigned char i = 0; i < 120; i++) gameboard[i] = 0;
	
	queue = 0;
	active_type = 0;
	
	// Initialize the queue with pieces
	for (int i = 0; i < 5; i++) t_next_piece();
	
	// Set starting level
	level = starting_level;
	
	gameover = 0;
	lines = 0;
	drop_count = 0;
	has_hit_bottom = 0;
	hold_delay = 0;
	hold_piece = 0;
	move_delay = 0;
	
	gfx_draw_number(4, 14, lines);
}



void t_left() {
	unsigned char can_move = 1;
	for (int i = 0; i < 4; i++) if (active_pos[i][0] == 0 || t_get_piece(active_pos[i][0] - 1, active_pos[i][1]) != 0) can_move = 0;
	if (can_move) {
		if (has_hit_bottom) drop_count = 0; // This is standard to allow sliding past when a piece would drop if you keep moving it
		for (int i = 0; i < 4; i++) --active_pos[i][0];
	}
}

void t_right() {
	unsigned char can_move = 1;
	for (int i = 0; i < 4; i++) if (active_pos[i][0] == 9 || t_get_piece(active_pos[i][0] + 1, active_pos[i][1]) != 0) can_move = 0;
	if (can_move) {
		if (has_hit_bottom) drop_count = 0; // This is standard to allow sliding past when a piece would drop if you keep moving it
		for (int i = 0; i < 4; i++) ++active_pos[i][0];
	}
}

/* This attempts to finalize the rotation by finding a
 * place where the piece can fit ("kicking" it) */
void t_finish_rotate(signed char p[4][2]) {
	if (active_type > 1) { // Not a square
		// Let's find our bottom, left, and right constraints of the old piece,
		// as well as the width and an index to observe as the bottom of the new piece
		signed char bot = active_pos[0][1], left = active_pos[0][0], right = active_pos[0][0];
		signed char n_bot = p[0][1], n_left = p[0][0], n_right = p[0][0];
		for (int i = 1; i < 4; i++) {
			if (active_pos[i][1] < bot) bot = active_pos[i][1];
			if (active_pos[i][0] < left) left = active_pos[i][0];
			if (active_pos[i][0] > right) right = active_pos[i][0];
			if (p[i][1] < n_bot) n_bot = p[i][1];
			if (p[i][0] < n_left) n_left = p[i][0];
			if (p[i][0] > n_right) n_right = p[i][0];
		}
		
		// Now we've gathered all the data, let's decide how to reposition the piece.
		signed char xd = 0, yd = 0;
		yd = bot - n_bot;
		xd = ((right + left) / 2 - (n_right - n_left) / 2) - n_left;
		
		// Check for intersections with boundaries
		if (n_left + xd < 0) xd += - (xd + n_left);
		if (n_right + xd > 9) xd -= (n_right + xd - 9);
		
		// Check for piece intersections (this is where kicking will eventually need to occur.)
		unsigned char intersects = 0;
		for (int i = 0; i < 4; i++) {
			if (t_get_piece(p[i][0] + xd, p[i][1] + yd)) intersects = 1;
		}
		
		if (intersects == 0) {
			for (int i = 0; i < 4; i++) {
				active_pos[i][0] = p[i][0] + xd;
				active_pos[i][1] = p[i][1] + yd;
			}
		}
	}
}

void t_tick(unsigned char *next_state, unsigned char ctrl) {
 	if (gameover) {
		 (*next_state) = 30;
		 return;
	}
	 
	unsigned char ctrl_now = control_get();
	if (ctrl_now & L && move_delay > -7) --move_delay;
	else if (ctrl_now & R && move_delay < 7) ++move_delay;
	else if ((ctrl_now & L) == 0 && (ctrl_now & R) == 0) move_delay = 0;
	
	
	if (ctrl & Start) { // Pause
		(*next_state) = 21;
		return;
	}
	
	if (ctrl & Select) { // Hold piece
		if (hold_delay == 0) {
			hold_delay = 2;
			unsigned char temp = hold_piece;
			hold_piece = active_type;
			if (temp != 0) {
				queue <<= 4;
				queue |= temp;
			}
			t_next_piece();
			draw_piece(4, 2, hold_piece);
		}
	}
	
	// Move pieces here
	if (ctrl & L || move_delay <= -7) {
		if (move_delay >= 0) move_delay = -1;
		t_left();
	} else if (ctrl & R || move_delay >= 7) {
		if (move_delay <= 0) move_delay = 1;
		t_right();
	}
	
	if (ctrl & A) { // ROR
		signed char p[4][2];
		for (int i = 0; i < 4; i++) { // Dumb rotation first, align in finish_rotate
			p[i][0] = active_pos[0][0] + (active_pos[i][1] - active_pos[0][1]);
			p[i][1] = active_pos[0][1] - (active_pos[i][0] - active_pos[0][0]);
		}
		t_finish_rotate(p);
	} else if (ctrl & B) { // Rotate Left
		signed char p[4][2];
		for (int i = 0; i < 4; i++) { // Dumb rotation first, align in finish_rotate
			p[i][0] = active_pos[0][0] - (active_pos[i][1] - active_pos[0][1]);
			p[i][1] = active_pos[0][1] + (active_pos[i][0] - active_pos[0][0]);
		}
		t_finish_rotate(p);
	} 
	
	// Lower the piece here	
	drop_count++;
	if (drop_count >= 11 - level || ctrl_now & D || ctrl & U) {
		drop_count = 0;
		do {
			// Check if the piece will collide with another piece or the bottom of the stage.
			for (int i = 0; i < 4; i++) {
				if (active_pos[i][1] == 0 || t_get_piece(active_pos[i][0], active_pos[i][1] - 1)) {
					// We've hit a piece or the bottom and cannot proceed
					if (has_hit_bottom == 0 && !(ctrl & U)) {
						// This is the first detection, but we are allowed a period of "sliding"
						has_hit_bottom = 1;
						return;
					} else {
						// This is the second detection, and so the piece must be committed in place.
						has_hit_bottom = 0;
						t_commit_piece();
						t_next_piece();
						return;
					}
				}
			}
			
			has_hit_bottom = 0;
			
			// If we've made it past collision detection, lower the piece by 1.
			for (int i = 0; i < 4; i++) {
				--active_pos[i][1];
			}
		} while(ctrl & U);
	}
}