
#ifndef INC_SPI_UTILS

#include <stm32f1xx_hal.h>

#define INC_SPI_UTILS

static inline HAL_StatusTypeDef mpu_write_register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg, uint8_t value)
{
    uint8_t tx[2];
    tx[0] = reg & 0x7F;
    tx[1] = value;

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef st = HAL_SPI_Transmit(hspi, tx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    return st;
}

static inline HAL_StatusTypeDef mpu_read_register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg, uint8_t *value)
{
    uint8_t tx = reg | 0x80;
    uint8_t rx = 0;

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef st = HAL_SPI_Transmit(hspi, &tx, 1, HAL_MAX_DELAY);
    if (st == HAL_OK) {
        st = HAL_SPI_Receive(hspi, &rx, 1, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    if (st == HAL_OK) {
        *value = rx;
    }
    return st;
}

#endif