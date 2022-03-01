
#ifndef _CONTROL_H_
#define _CONTROL_H_

// Controller bits
#define A 0x80
#define B 0x40
#define Select 0x20
#define Start 0x10
#define U 0x08
#define D 0x04
#define L 0x02
#define R 0x01

unsigned char control_get();

#endif