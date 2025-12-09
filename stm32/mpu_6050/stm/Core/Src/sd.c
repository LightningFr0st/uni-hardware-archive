
#include "sd.h"
const uint32_t transmit_timeout = 10000;
uint8_t tx_ff[512] = { [0 ... 511] = 0xFF };
volatile uint8_t spi_done = 0;

static void SD_Select() {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}

void SD_Unselect() {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

/*
R1: 0abcdefg
     ||||||`- 1th bit (g): card is in idle state
     |||||`-- 2th bit (f): erase sequence cleared
     ||||`--- 3th bit (e): illigal command detected
     |||`---- 4th bit (d): crc check error
     ||`----- 5th bit (c): error in the sequence of erase commands
     |`------ 6th bit (b): misaligned addres used in command
     `------- 7th bit (a): command argument outside allowed range
             (8th bit is always zero)
*/
static uint8_t SD_ReadR1() {
    uint8_t r1;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    for(;;) {
        HAL_SPI_TransmitReceive(&SD_SPI_PORT, &tx, &r1, sizeof(r1), HAL_MAX_DELAY);
        if((r1 & 0x80) == 0) // 8th bit alwyas zero, r1 recevied
            break;
    }
    return r1;
}

// data token for CMD9, CMD17, CMD18 and CMD24 are the same
#define DATA_TOKEN_CMD9  0xFE
#define DATA_TOKEN_CMD17 0xFE
#define DATA_TOKEN_CMD18 0xFE
#define DATA_TOKEN_CMD24 0xFE
#define DATA_TOKEN_CMD25 0xFC

static int SD_WaitDataToken(uint8_t token) {
    uint8_t fb;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    for(;;) {
        HAL_SPI_TransmitReceive(&SD_SPI_PORT, &tx, &fb, sizeof(fb), HAL_MAX_DELAY);
        if(fb == token)
            break;

        if(fb != 0xFF)
            return -1;
    }
    return 0;
}

static int SD_ReadBytes(uint8_t* buff, size_t buff_size, uint32_t buf_timeout) {
    //* dma
    spi_done = 0;
    if(HAL_SPI_TransmitReceive(&SD_SPI_PORT, tx_ff, buff, buff_size, HAL_MAX_DELAY) != HAL_OK)
    {
        return -1;
    }

//    uint32_t spi_start_time = HAL_GetTick();
//		while(!spi_done) {
//				uint32_t curr_time = HAL_GetTick();
//        if((curr_time - spi_start_time) > buf_timeout) {
//            HAL_SPI_Abort(&SD_SPI_PORT);
//            return -2; // timeout
//        }
//        __WFI();
//		}

    return 0;
}

static int SD_WaitNotBusy() {
    uint8_t busy;
    do {
        if(SD_ReadBytes(&busy, sizeof(busy), transmit_timeout) < 0) {
            return -1;
        }
    } while(busy != 0xFF);

    return 0;
}
 
int SD_Init() {
    /*
    Step 1.

    Set DI and CS high and apply 74 or more clock pulses to SCLK. Without this
    step under certain circumstances SD-card will not work. For instance, when
    multiple SPI devices are sharing the same bus (i.e. MISO, MOSI, CS).
    */
    SD_Unselect();

    uint8_t high = 0xFF;
    for(int i = 0; i < 10; i++) { // 80 clock pulses
        HAL_SPI_Transmit(&SD_SPI_PORT, &high, sizeof(high), HAL_MAX_DELAY);
    }

    SD_Select();

    /*
    Step 2.
    
    Send CMD0 (GO_IDLE_STATE): Reset the SD card.
    */
    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SD_ReadR1() != 0x01) {
        SD_Unselect();
        return -1;
    }

    /*
    Step 3.

    After the card enters idle state with a CMD0, send a CMD8 with argument of
    0x000001AA and correct CRC prior to initialization process. If the CMD8 is
    rejected with illigal command error (0x05), the card is SDC version 1 or
    MMC version 3. If accepted, R7 response (R1(0x01) + 32-bit return value)
    will be returned. The lower 12 bits in the return value 0x1AA means that
    the card is SDC version 2 and it can work at voltage range of 2.7 to 3.6
    volts. If not the case, the card should be rejected.
    */
    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x08 /* CMD8 */, 0x00, 0x00, 0x01, 0xAA /* ARG */, (0x43 << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SD_ReadR1() != 0x01) {
        SD_Unselect();
        return -2; // not an SDHC/SDXC card (i.e. SDSC, not supported)
    }

    {
        uint8_t resp[4];
        if(SD_ReadBytes(resp, sizeof(resp), transmit_timeout) < 0) {
            SD_Unselect();
            return -3;
        }

        if(((resp[2] & 0x01) != 1) || (resp[3] != 0xAA)) {
            SD_Unselect();
            return -4;
        }
    }

    /*
    Step 4.

    And then initiate initialization with ACMD41 with HCS flag (bit 30).
    */
    for(;;) {
        if(SD_WaitNotBusy() < 0) { // keep this!
            SD_Unselect();
            return -1;
        }

        {
            static const uint8_t cmd[] =
                { 0x40 | 0x37 /* CMD55 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
        }

        if(SD_ReadR1() != 0x01) {
            SD_Unselect();
            return -5;
        }

        if(SD_WaitNotBusy() < 0) { // keep this!
            SD_Unselect();
            return -1;
        }

        {
            static const uint8_t cmd[] =
                { 0x40 | 0x29 /* ACMD41 */, 0x40, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
        }

        uint8_t r1 = SD_ReadR1();
        if(r1 == 0x00) {
            break;
        }

        if(r1 != 0x01) {
            SD_Unselect();
            return -6;
        }
    }

    /*
    Step 5.

    After the initialization completed, read OCR register with CMD58 and check
    CCS flag (bit 30). When it is set, the card is a high-capacity card known
    as SDHC/SDXC.
    */
    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x3A /* CMD58 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SD_ReadR1() != 0x00) {
        SD_Unselect();
        return -7;
    }

    {
        uint8_t resp[4];
        if(SD_ReadBytes(resp, sizeof(resp), transmit_timeout) < 0) {
            SD_Unselect();
            return -8;
        }

        if((resp[0] & 0xC0) != 0xC0) {
            SD_Unselect();
            return -9;
        }
    }

    SD_Unselect();
    return 0;
}



int SD_ReadSingleBlock(uint32_t blockNum, uint8_t* buff) {
    uint8_t crc[2];

    SD_Select();

    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    /* CMD17 (SEND_SINGLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x11 /* CMD17 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SD_ReadR1() != 0x00) {
        SD_Unselect();
        return -2;
    }

    if(SD_WaitDataToken(DATA_TOKEN_CMD17) < 0) {
        SD_Unselect();
        return -3;
    }

    if(SD_ReadBytes(buff, 512, transmit_timeout) < 0) {
        SD_Unselect();
        return -4;
    }

    if(SD_ReadBytes(crc, 2, transmit_timeout) < 0) {
        SD_Unselect();
        return -5;
    }

    SD_Unselect();
    return 0;
}

int SD_ReadBegin(uint32_t blockNum) {
    SD_Select();

    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    /* CMD18 (READ_MULTIPLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x12 /* CMD18 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SD_ReadR1() != 0x00) {
        SD_Unselect();
        return -2;
    }

    SD_Unselect();
    return 0;
}

int SD_ReadData(uint8_t* buff) {
    uint8_t crc[2];
    SD_Select();

    if(SD_WaitDataToken(DATA_TOKEN_CMD18) < 0) {
        SD_Unselect();
        return -1;
    }

    if(SD_ReadBytes(buff, 512, transmit_timeout) < 0) {
        SD_Unselect();
        return -2;
    }

    if(SD_ReadBytes(crc, 2, transmit_timeout) < 0) {
        SD_Unselect();
        return -3;
    }

    SD_Unselect();
    return 0;

}

int SD_ReadEnd() {
    SD_Select();

    /* CMD12 (STOP_TRANSMISSION) */
    {
        static const uint8_t cmd[] = { 0x40 | 0x0C /* CMD12 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 };
        HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    /*
    The received byte immediataly following CMD12 is a stuff byte, it should be
    discarded before receive the response of the CMD12
    */
    uint8_t stuffByte;
    if(SD_ReadBytes(&stuffByte, sizeof(stuffByte), transmit_timeout) < 0) {
        SD_Unselect();
        return -1;
    }

    if(SD_ReadR1() != 0x00) {
        SD_Unselect();
        return -2;
    }
    
    SD_Unselect();
    return 0;
}



int SD_StartWriteBlock(uint32_t blockNum, const uint8_t* buff, uint32_t buf_timeout) {
    SD_Select();

    if(SD_WaitNotBusy() < 0) { // keep this!
        SD_Unselect();
        return -1;
    }

    /* CMD24 (WRITE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x18 /* CMD24 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    HAL_SPI_Transmit(&SD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SD_ReadR1() != 0x00) {
        SD_Unselect();
        return -2;
    }

    uint8_t dataToken = DATA_TOKEN_CMD24;
    HAL_SPI_Transmit(&SD_SPI_PORT, &dataToken, sizeof(dataToken), HAL_MAX_DELAY);
    //* dma
    if(HAL_SPI_Transmit(&SD_SPI_PORT, buff, 512, HAL_MAX_DELAY) != HAL_OK)
    {
        return -1;
    }
    return HAL_OK;
}

int SD_FinishWriteBlock() {
    uint8_t crc[2] = { 0xFF, 0xFF };

    HAL_SPI_Transmit(&SD_SPI_PORT, crc, sizeof(crc), HAL_MAX_DELAY);

    /*
        dataResp:
        xxx0abc1
            010 - Data accepted
            101 - Data rejected due to CRC error
            110 - Data rejected due to write error
    */
    uint8_t dataResp;
    SD_ReadBytes(&dataResp, sizeof(dataResp), transmit_timeout);
    if((dataResp & 0x1F) != 0x05) { // data rejected
        SD_Unselect();
        return -3;
    }

    if(SD_WaitNotBusy() < 0) {
        SD_Unselect();
        return -4;
    }

    SD_Unselect();
    return 0;
}





void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &SD_SPI_PORT) {
        spi_done = 1; 
    }
}



