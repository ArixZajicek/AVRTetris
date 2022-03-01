#include "avr.h"
#include "control.h"

unsigned char control_get() {
	/* Set outputs properly: 
	 * CLOCK: PC0 - Output
	 * LATCH: PC1 - Output
	 * DATA:  PC2 - Input */
	DDRC = 0x03;
	PORTC = 0x02;
	for (int d = 0; d < 12 * 4; d++) NOP();
	PORTC = 0x00;
	unsigned char data = ((PINC & 0x04) >> 2);
	
	for (int d = 0; d < 6 * 4; d++) NOP();
	
	// A B Sel Start Up Down Left Right
	for (int i = 0; i < 7; i++) {
		PORTC = 0x01;
		for (int d = 0; d < 6 * 4; d++) NOP();
		PORTC = 0x00;
		data = data << 1;
		data = data | ((PINC & 0x04) >> 2);
		for (int d = 0; d < 6 * 4; d++) NOP();
	}
	
	return ~data;
}