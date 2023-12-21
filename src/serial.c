#include <avr/io.h>

#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void serial_initialize(int baud) {
	// enable "double the USART transmission speed"
	UCSR0A = 1 << U2X0;

	// set the baud rate
	const uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
	UBRR0L = baud_setting;
	UBRR0H = baud_setting >> 8;

	set_bit(UCSR0B, RXEN0); // enable UART Rx
	set_bit(UCSR0B, TXEN0); // enable UART Tx
	set_bit(UCSR0B, RXCIE0); // enable receive interrupts
	clear_bit(UCSR0B, UDRIE0); // disable data register empty interrupts
}
