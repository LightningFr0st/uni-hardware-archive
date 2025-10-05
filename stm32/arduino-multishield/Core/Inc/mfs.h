#include "main.h"

#define MFS_SET_LEDS(STATE) \
				HAL_GPIO_WritePin(D2_led_GPIO_Port, D2_led_Pin, STATE); \
				HAL_GPIO_WritePin(D3_led_GPIO_Port, D3_led_Pin, STATE); \
				HAL_GPIO_WritePin(D4_led_GPIO_Port, D4_led_Pin, STATE) \
				
void MFS_DisplayNumber(uint16_t number);

void MFS_DisplayStop();

void MFS_RefreshDisplay();

extern volatile uint16_t display_number;