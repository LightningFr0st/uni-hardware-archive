
#ifndef __SD_H__
#define __SD_H__

#include "stm32f1xx_hal.h"
#include "main.h"

#define SD_SPI_PORT hspi2

#define SD_CHUNK_SIZE 512 
#define START_BLOCK_NUM 0x00ABCD

extern SPI_HandleTypeDef SD_SPI_PORT;
extern const uint32_t transmit_timeout;

// call before initializing any SPI devices
void SD_Unselect();

// all procedures return 0 on success, < 0 on failure

int SD_Init();
int SD_ReadSingleBlock(uint32_t blockNum, uint8_t* buff);

int SD_StartWriteBlock(uint32_t blockNum, const uint8_t* buff, uint32_t buf_timeout);
int SD_FinishWriteBlock();

int SD_ReadData(uint8_t* buff);

int SD_ReadEnd();

int SD_ReadBegin(uint32_t blockNum);

#endif // __SD_H__
