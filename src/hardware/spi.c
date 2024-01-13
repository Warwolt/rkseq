#include "hardware/spi.h"

#include "util/bits.h"

#include <avr/io.h>

spi_t spi_initialize(spi_data_order_t data_order) {
	/* Configure pins */
	set_bit(DDRB, 3); // Set MOSI pin to output
	clear_bit(DDRB, 4); // Set MISO pin to input
	set_bit(DDRB, 5); // Set clock pin to output
	set_bit(DDRB, 2); // Set slave select to output (otherwise SPI becomes slave mode)

	/* Configure SPI */
	set_bit(SPCR, MSTR); // Master mode
	write_bit(SPCR, DORD, data_order); // Select order bits are sent
	set_bit(SPCR, SPE); // SPI Enable (must be last!)

	return (spi_t) {};
}

uint8_t spi_send(spi_t spi, uint8_t byte) {
	SPDR = byte; // start transmission
	while (!(SPSR & (1 << SPIF))) { // wait for transmission to completele
	}
	return SPDR;
}

uint8_t spi_receive(spi_t spi) {
	return spi_send(spi, 0);
}
