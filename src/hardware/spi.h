#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef enum {
	SPI_DATA_ORDER_MSB_FIRST = 0,
	SPI_DATA_ORDER_LSB_FIRST = 1,
} SpiDataOrder;

typedef struct {
	int: 0;
} Spi;

Spi Spi_initialize(SpiDataOrder data_order);
uint8_t Spi_send(Spi spi, uint8_t byte);
uint8_t Spi_receive(Spi spi);

#endif /* SPI_H */
