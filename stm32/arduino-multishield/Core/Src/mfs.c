#include "mfs.h"

volatile uint16_t display_number = 0u;

const uint8_t digit_patterns[10] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7 
    0x80, // 8 
    0x90, // 9 
};

#define DIGITS_COUNT 4u

const uint8_t digit_select[DIGITS_COUNT] = {
    0xF1,
    0xF2,
    0xF4,
    0xF8
};

uint8_t display_buffer[4] = {0};

static void shiftOut(uint8_t data) {
    for (int8_t i = 7; i >= 0; i--) {
        if (data & (1 << i)) 
            HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_SET);
				else 
            HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
    }
}

void MFS_DisplayNumber(uint16_t number) {   
    display_buffer[0] = digit_patterns[number / 1000u];
    display_buffer[1] = digit_patterns[(number % 1000u) / 100u];
    display_buffer[2] = digit_patterns[(number % 100u) / 10u];
    display_buffer[3] = digit_patterns[number % 10u];
    if (number < 1000) 
			display_buffer[0] = 0xFF;
    if (number < 100) 
			display_buffer[1] = 0xFF;
    if (number < 10) 
			display_buffer[2] = 0xFF;
}

void MFS_DisplayStop()
{
	display_buffer[0] = 0x92;
	display_buffer[1] = 0xCE;
	display_buffer[2] = 0xC0;
	display_buffer[3] = 0x8C;
}

void MFS_RefreshDisplay(void) {

		uint8_t current_digit = 0u;
		while(current_digit < DIGITS_COUNT)
		{
			HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, GPIO_PIN_RESET);
			shiftOut(display_buffer[current_digit]);
			shiftOut(digit_select[current_digit]);
			current_digit++;
			HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, GPIO_PIN_SET);
		}
}