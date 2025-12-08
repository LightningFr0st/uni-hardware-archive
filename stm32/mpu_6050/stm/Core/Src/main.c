/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"

#include "mpu6050.h"

#include "sd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#define MPU6050_Address 0xD0
//#define PWR_MGMT_1_REG 0x6B
//#define SMPLRT_DIV_REG 0x19
//#define ACCEL_CONFIG_REG 0x1B
//#define GYRO_CONFIG_REG 0x1C
//#define INT_ENABLE_REG 0x38
//#define INT_PIN_CFG_REG 0x37
//#define ACCEL_XOUT_H_REG 0x3B
//#define INT_STATUS 0x3A

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// ==== SPI-?????? ?????? ? MPU6500 ====

#define ACCEL_XOUT_H_REG 0x3B
#define NUM_MPU_SPI 5

typedef struct {
    MPU6500_t mpu_state;
	
		// CS info
	  GPIO_TypeDef *cs_port;
    uint16_t      cs_pin;
    
} MPUDescriptor;

typedef struct __attribute__((packed)) {
    uint8_t mpu_id;
    float   kx; // roll
    float   ky; // pitch
} LogPacket_t;

#define LOG_BUFFER_SIZE 1024
#define MAX_BLOCK_BYTES_NUM 504
#define TXT_BUF_SIZE 1024
#define PACKET_SIZE 9
#define NUM_CHUNKS 2

static char txt_buf[TXT_BUF_SIZE];

typedef enum{
	NONE,
	WRITE,
	READ,
	SIZE
} ProgState_t;

typedef enum { 
  CHUNK_FREE = 0,
  CHUNK_COMMITTED = 1, 
  CHUNK_LOCKED = -1,
  CHUNK_INIT = 2,
} Chunk_state_t;

typedef enum { 
  PUSH_INF = CHUNK_FREE, 
  POP_INF = CHUNK_COMMITTED,
  _MAIN_OPRTN_NUM = 2,
  PEEK_INF = 3
} Buf_data_operation;


typedef struct {
    uint8_t  data[LOG_BUFFER_SIZE];
    uint16_t buf_ptrs[_MAIN_OPRTN_NUM];
	
		volatile uint8_t states_of_chunks[NUM_CHUNKS]; // 1 chunk - free, 2 - ...
    volatile uint8_t expected_chunks[_MAIN_OPRTN_NUM]; // there are 2 states (either readable or writable)

	
} RingBuffer_t;


static volatile ProgState_t curr_state = NONE;
static volatile uint8_t sd_initialized = 0; //*
static volatile uint8_t is_changed_state = 0;
static volatile uint32_t sd_write_block_counter = 0;
static volatile uint32_t sd_read_block_counter = 0; //*
static volatile uint8_t sd_read_complete = 0;

static MPUDescriptor mpu_descriptors[5] = {
    { {0}, CS0_GPIO_Port, CS0_Pin },
    { {0}, CS1_GPIO_Port, CS1_Pin },
    { {0}, CS2_GPIO_Port, CS2_Pin },
    { {0}, CS3_GPIO_Port, CS3_Pin },
		{ {0}, CS4_GPIO_Port, CS4_Pin },
};

static RingBuffer_t log_rb;



static void rb_init(RingBuffer_t *rb)
{
    for (int i=0; i<_MAIN_OPRTN_NUM; i++)
    {
      rb->buf_ptrs[i] = CHUNK_FREE;
      rb->expected_chunks[i] = 0; 
    } 

    for (int i=0; i<NUM_CHUNKS; i++)
    {
      rb->states_of_chunks[i] = CHUNK_FREE;
    }

}

static inline uint32_t chunk_start_ind(uint8_t chunk_id)
{ 
  return (uint32_t)chunk_id * SD_CHUNK_SIZE; 
}

static inline uint32_t chunk_end_bound(uint8_t chunk_id)
{ 
  return chunk_start_ind(chunk_id) + MAX_BLOCK_BYTES_NUM; //*? MAX_BLOCK_BYTES_NUM
}

static inline uint8_t next_chunk(uint8_t chunk_id)
{ 
  return (chunk_id + 1) % NUM_CHUNKS; 
}


static int16_t rb_manage_bytes(
  RingBuffer_t *rb,
  uint8_t * data,
  uint16_t len,
  Buf_data_operation oprtn_detailed
)
{
  Buf_data_operation oprtn_type = oprtn_detailed % 2;
  if (len == 0 || len > MAX_BLOCK_BYTES_NUM)
    return 0;

  uint8_t operated_chunk = rb->expected_chunks[oprtn_type]; 

  if (rb->states_of_chunks[operated_chunk] != oprtn_type) 
    return 0;


  uint32_t start = chunk_start_ind(operated_chunk);

  uint32_t bound = chunk_end_bound(operated_chunk);
	uint16_t* bufIndPtr = &rb->buf_ptrs[oprtn_type];
	uint16_t bufInd = *bufIndPtr;

  if (!(bufInd >= start && bufInd < bound)) { 

    bufInd = (uint16_t) start;
    *bufIndPtr = bufInd;

  }

  uint32_t offset_in_chunk = bufInd - start;
  uint32_t avail = MAX_BLOCK_BYTES_NUM - offset_in_chunk;

  uint16_t res;
  if(oprtn_type == PUSH_INF)
  {
    if (len > avail) 
    {
      if(len < PACKET_SIZE)
      {
        rb->states_of_chunks[operated_chunk] = oprtn_type-1;
        return CHUNK_LOCKED;
      }
      return 0;

    }
      
    for (uint16_t i = 0; i < len; ++i) {
        rb->data[bufInd++] = data[i];
    }
    //*bufIndPtr = bufInd;
    res = 1;
  }
  else
  {
    if (len > avail) 
      len = avail;
    for (uint16_t i = 0; i < len; ++i) {
      if(oprtn_detailed == PEEK_INF)
      {
        // *bufIndPtr = bufInd; 
        return len;
      }
      if(data != NULL)
      {
        data[i] = rb->data[bufInd++];
      }
    }
    //*bufIndPtr = bufInd;
    res = len;
  }

  if (bufInd == bound) {

    rb->states_of_chunks[operated_chunk] = oprtn_type-1; 
    
    // toggle to next chunk
    uint8_t next_chunk_ind = next_chunk(operated_chunk);

    rb->expected_chunks[oprtn_type] = next_chunk_ind;

    *bufIndPtr = (uint16_t) chunk_start_ind(next_chunk_ind);

  }
  else
  {
    *bufIndPtr = bufInd;
  }

  return res; 

}


static int16_t log_push_packet(uint8_t mpu_idx, const MPU6500_t *mpu_state)
{
	
    LogPacket_t pkt;
    pkt.mpu_id = mpu_idx;
    pkt.kx = (float)mpu_state->KalmanAngleX;
    pkt.ky = (float)mpu_state->KalmanAngleY;

    return rb_manage_bytes(&log_rb, (uint8_t *) &pkt, sizeof(pkt), PUSH_INF);
}




static HAL_StatusTypeDef mpu_spi_read_regs(MPUDescriptor *s, uint8_t reg, uint8_t *dst, uint16_t len)
{
    uint8_t cmd = reg | 0x80;

    HAL_GPIO_WritePin(s->cs_port, s->cs_pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef st = HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    if (st == HAL_OK) {
        st = HAL_SPI_Receive(&hspi1, dst, len, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(s->cs_port, s->cs_pin, GPIO_PIN_SET);

    return st;
}

static void bytes_to_hex(const uint8_t* src, size_t len, char* dst) {
    static const char hexdig[] = "0123456789ABCDEF";
    for (size_t i = 0; i < len; ++i) {
        uint8_t b = src[i];
        dst[2*i + 0] = hexdig[(b >> 4) & 0xF];
        dst[2*i + 1] = hexdig[b & 0xF];
    }
}

static void process_and_send(uint8_t* buf) {
	
		uint8_t pkt[17];
    pkt[0] = (0);

    pkt[1] = buf[1];  pkt[2] = buf[0];   // AX
    pkt[3] = buf[3];  pkt[4] = buf[2];   // AY
    pkt[5] = buf[5];  pkt[6] = buf[4];   // AZ
	
    pkt[7]  = buf[9];  pkt[8]  = buf[8];   // GX
    pkt[9]  = buf[11]; pkt[10] = buf[10];  // GY
    pkt[11] = buf[13]; pkt[12] = buf[12];  // GZ
  
    uint32_t t = HAL_GetTick();
    pkt[13] = (uint8_t)(t);
    pkt[14] = (uint8_t)(t >> 8);
    pkt[15] = (uint8_t)(t >> 16);
    pkt[16] = (uint8_t)(t >> 24);
	
		char line[17*2 + 2];
    bytes_to_hex(pkt, sizeof(pkt), line);
    line[34] = '\r';
    line[35] = '\n';

    HAL_UART_Transmit(&huart2, (uint8_t*)line, 36, 1000);
}

static volatile uint16_t current_chunk;

void dma_callback(void)
{
	current_chunk = 1;
}



void parse_and_send_packets(const uint8_t *data, uint16_t len)
{
    const size_t pkt_size = sizeof(LogPacket_t);
    uint16_t count = len / pkt_size;
    if (count == 0) 
      return;
  
    char *out = txt_buf;
    size_t rem = TXT_BUF_SIZE;

    for (uint16_t i = 0; i < count; ++i) {
        LogPacket_t pkt;
        memcpy(&pkt, data + i * pkt_size, pkt_size);

        int need = snprintf(out, rem, "#%d#%.4f#%.4f\r\n",
                            pkt.mpu_id, pkt.kx, pkt.ky);

        if (need < 0) {
            break;
        }

        if ((size_t)need >= rem) {
            HAL_UART_Transmit(&huart2, (uint8_t*)txt_buf, (uint16_t)(TXT_BUF_SIZE - rem), HAL_MAX_DELAY);
            // сброс
            out = txt_buf;
            rem = TXT_BUF_SIZE;
        }
        out += need;
        rem -= (size_t)need;
    }

    // отправляем остаток если есть
    size_t used = TXT_BUF_SIZE - rem;
    if (used > 0) {
        HAL_UART_Transmit(&huart2, (uint8_t*)txt_buf, (uint16_t)used, HAL_MAX_DELAY);
    }
}

static uint16_t log_peek_chunk(uint8_t **pIndPtr)
{
    *pIndPtr = log_rb.data + log_rb.buf_ptrs[POP_INF];
    return rb_manage_bytes(&log_rb, NULL, SD_CHUNK_SIZE, PEEK_INF);
}

//static void log_flush_half(void) //*
//{
//    if (rb_size(&log_rb) >= SD_CHUNK_SIZE) {
//				
//        uint8_t out[SD_CHUNK_SIZE];
//        uint16_t n = rb_pop_bytes(&log_rb, out, SD_CHUNK_SIZE);
//        if (n > 0) 
//				{	
//          //* PARSE
//            HAL_UART_Transmit(&huart2, out, n, HAL_MAX_DELAY);
//        }
//    }
//}


static void send_mpu_data(uint8_t idx)
{	
		MPU6500_t* mpu_state = &mpu_descriptors[idx].mpu_state;
	
    char buf[64];
	  
		uint32_t now = HAL_GetTick();

		int len = snprintf(buf, sizeof(buf),
											 "#%d#%.4f#%.4f\r\n",
											 idx,
											 mpu_state->KalmanAngleX,
                       mpu_state->KalmanAngleY);
		
    if (len > 0 && len < (int)sizeof(buf)) 
		{
        HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
    }
}


static void poll_loop_spi(void)
{
		uint8_t raw_data[14];
		memset(raw_data, 0, sizeof(raw_data));
	
		while (1) 
		{
        for (int i = 0; i < NUM_MPU_SPI; ++i) {
            if (mpu_spi_read_regs(&mpu_descriptors[i], ACCEL_XOUT_H_REG, raw_data, 14) == HAL_OK) 
						{
              switch(curr_state){
                case NONE:
                  process_mpu_data(&mpu_descriptors[i].mpu_state, raw_data);
                  //log_push_packet(i, &mpu_descriptors[i].mpu_state);	
                  send_mpu_data(i);
                  break;
                case WRITE:
                  if(is_changed_state)
                  {
                    rb_init(&log_rb);
                    memset(log_rb.data, 0, sizeof(log_rb.data));
                    is_changed_state = 0u;
                  } 
                  process_mpu_data(&mpu_descriptors[i].mpu_state, raw_data);	
                  int16_t res = log_push_packet(i, &mpu_descriptors[i].mpu_state);
                  if(res == CHUNK_LOCKED || curr_state == READ)
                  {
                    uint8_t * peekedIndPtr;
                    uint16_t peekedLength = log_peek_chunk(&peekedIndPtr);
                    if (peekedLength > 0) 
                    {	
                        // SD
                        parse_and_send_packets(peekedIndPtr, peekedLength);
                    }
                    memset(log_rb.data, 0, sizeof(log_rb.data));
                  }
                  break;
                case READ:
                  if(is_changed_state)  
                  { 
                    rb_init(&log_rb);
                    memset(log_rb.data, 0, sizeof(log_rb.data));
                    is_changed_state = 0u;
                  } 

                  // SD READ
                  uint8_t * peekedIndPtr;
                  uint16_t peekedLength = log_peek_chunk(&peekedIndPtr);
                    if (peekedLength > 0) 
                    {	
                        parse_and_send_packets(peekedIndPtr, peekedLength);
                    }
                    memset(log_rb.data, 0, sizeof(log_rb.data));
                  break;
              }

                // process_mpu_data(&mpu_descriptors[i].mpu_state, raw_data);
								// 		log_push_packet(i, &mpu_descriptors[i].mpu_state);	
								// 		send_mpu_data(i);
								
            }
        }
    }
}

static const uint32_t StartBlockNum = 0x00ABCD;

static void initSD(void)
{
	int sdInitCode;
	sdInitCode	= SD_Init(); //add error logic
	uint32_t blocksNum;
	sdInitCode = SD_GetBlocksNumber(&blocksNum); //add error logic	
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
 
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
		
	initSD();
	
	
	rb_init(&log_rb);
	
  for (int i = 0; i < NUM_MPU_SPI; ++i) {
      init_mpu(&hspi1, mpu_descriptors[i].cs_port, mpu_descriptors[i].cs_pin);
  }
	HAL_Delay(100);
	for (int i = 0; i < NUM_MPU_SPI; ++i) 
	{
		mpu_descriptors[i].mpu_state.timer = HAL_GetTick();
		
		Kalman_Init(&mpu_descriptors[i].mpu_state.KalmanX);
		Kalman_Init(&mpu_descriptors[i].mpu_state.KalmanY);
	}
	
  poll_loop_spi();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  while (1)
//  {		
//		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SD_CS_Pin|CS4_Pin|CS3_Pin|CS2_Pin
                          |CS1_Pin|CS0_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : STATE_SWITCH_Pin */
  GPIO_InitStruct.Pin = STATE_SWITCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(STATE_SWITCH_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SD_CS_Pin CS4_Pin CS3_Pin CS2_Pin
                           CS1_Pin CS0_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin|CS4_Pin|CS3_Pin|CS2_Pin
                          |CS1_Pin|CS0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
