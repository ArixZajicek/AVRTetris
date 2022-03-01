/* This class manages the tetris board, dropping pieces, new pieces, held piece, etc. */

#ifndef _TETRIS_H_
#define _TETRIS_H_

/* Initialize the board area to be empty */
void t_init(unsigned char starting_level);

/* Draw the board at a starting position */
void t_draw_board();

/* Tick! */
void t_tick(unsigned char *next_state, unsigned char controller);

/* Fairly important. */
unsigned char t_random();

#endif