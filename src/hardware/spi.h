#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef enum {
	SPI_DATA_ORDER_MSB_FIRST = 0,
	SPI_DATA_ORDER_LSB_FIRST = 1,
} spi_data_order_t;

void spi_initialize(spi_data_order_t data_order);
void spi_send(uint8_t byte);

#endif /* SPI_H */
