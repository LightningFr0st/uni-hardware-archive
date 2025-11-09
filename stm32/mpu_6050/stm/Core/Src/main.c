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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MPU6050_Address 0xD0
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1B
#define GYRO_CONFIG_REG 0x1C
#define INT_ENABLE_REG 0x38
#define INT_PIN_CFG_REG 0x37
#define ACCEL_XOUT_H_REG 0x3B
#define INT_STATUS 0x3A

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c2_rx;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define MPU_ADDR_AD0_LOW   0xD0
#define MPU_ADDR_AD0_HIGH  0xD2

typedef struct {
    I2C_HandleTypeDef *i2c;
    uint16_t addr;
    uint8_t  buf[14];
    volatile uint8_t busy;
    volatile uint8_t ready;
	  uint8_t bus_id;
    uint8_t slot_id;
} mpu_t;

mpu_t mpus[4] = {
    { .i2c=&hi2c1, .addr=MPU_ADDR_AD0_LOW,  .busy=0, .ready=0, .bus_id=1, .slot_id=0 },
    { .i2c=&hi2c1, .addr=MPU_ADDR_AD0_HIGH, .busy=0, .ready=0, .bus_id=1, .slot_id=1 },
    { .i2c=&hi2c2, .addr=MPU_ADDR_AD0_LOW,  .busy=0, .ready=0, .bus_id=2, .slot_id=0 },
    { .i2c=&hi2c2, .addr=MPU_ADDR_AD0_HIGH, .busy=0, .ready=0, .bus_id=2, .slot_id=1 },
};

typedef struct {
    I2C_HandleTypeDef* i2c;
    int cur_idx;
    int next_idx;
    int members[2];
} bus_ctx_t;

bus_ctx_t buses[2];

static void init_bus_ctx(void) {
    buses[0].i2c = &hi2c1; buses[0].cur_idx = -1; buses[0].next_idx = 0;
    buses[1].i2c = &hi2c2; buses[1].cur_idx = -1; buses[1].next_idx = 0;
    int b1=0,b2=0;
    for (int i=0;i<4;i++) {
        if (mpus[i].i2c == &hi2c1) buses[0].members[b1++] = i;
        else                       buses[1].members[b2++] = i;
    }
}

static inline void start_read(int idx) {
    mpu_t* s = &mpus[idx];
    if (HAL_I2C_GetState(s->i2c) != HAL_I2C_STATE_READY) return;
    if (s->busy) return;
    if (HAL_I2C_Mem_Read_DMA(s->i2c, s->addr, ACCEL_XOUT_H_REG,
                             I2C_MEMADD_SIZE_8BIT, s->buf, 14) == HAL_OK) {
        s->busy = 1;
        bus_ctx_t* bc = (s->i2c == &hi2c1) ? &buses[0] : &buses[1];
        bc->cur_idx = idx;
    }
}

static void MPU_Init(I2C_HandleTypeDef *bus, uint16_t addr8) 
{
	uint8_t v;
	HAL_I2C_Mem_Read (&hi2c1, addr8, 0x75, 1, &v, 1, 1000);

	v = 0x00; 
	HAL_I2C_Mem_Write(bus, addr8, 0x6B, 1, &v, 1, 100);

	v = 0x07; 
	HAL_I2C_Mem_Write(bus, addr8, 0x19, 1, &v, 1, 100);

	v = 0x00; 
	HAL_I2C_Mem_Write(bus, addr8, 0x1B, 1, &v, 1, 100);
	
	v = 0x00; 
	HAL_I2C_Mem_Write(bus, addr8, 0x1C, 1, &v, 1, 100);
}


void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    bus_ctx_t* bc = (hi2c == &hi2c1) ? &buses[0] : &buses[1];
    int idx = bc->cur_idx;
    if (idx >= 0) {
        mpus[idx].busy  = 0;
        mpus[idx].ready = 1;
        bc->cur_idx = -1;
    }
}

static inline uint8_t i2c_bus_bit(const mpu_t* s) {
    return (s->i2c == &hi2c2) ? 1u : 0u;
}
static inline uint8_t addr_bit(const mpu_t* s) {
    return (s->addr == MPU_ADDR_AD0_HIGH) ? 1u : 0u; // 0x69<<1
}

static void bytes_to_hex(const uint8_t* src, size_t len, char* dst) {
    static const char hexdig[] = "0123456789ABCDEF";
    for (size_t i = 0; i < len; ++i) {
        uint8_t b = src[i];
        dst[2*i + 0] = hexdig[(b >> 4) & 0xF];
        dst[2*i + 1] = hexdig[b & 0xF];
    }
}

static void process_and_send(int idx) {
    mpu_t* s = &mpus[idx];
	
		uint8_t pkt[17];
    pkt[0] = (i2c_bus_bit(s) & 1u) | ((addr_bit(s) & 1u) << 1);

    pkt[1] = s->buf[1];  pkt[2] = s->buf[0];   // AX
    pkt[3] = s->buf[3];  pkt[4] = s->buf[2];   // AY
    pkt[5] = s->buf[5];  pkt[6] = s->buf[4];   // AZ
	
    pkt[7]  = s->buf[9];  pkt[8]  = s->buf[8];   // GX
    pkt[9]  = s->buf[11]; pkt[10] = s->buf[10];  // GY
    pkt[11] = s->buf[13]; pkt[12] = s->buf[12];  // GZ
  
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
	
    s->ready = 0;
}

void poll_loop(void)
{
    for (;;) {
        for (int b=0;b<2;b++) {
            bus_ctx_t* bc = &buses[b];
            if (bc->cur_idx < 0) {
                int i0 = bc->members[bc->next_idx];
                start_read(i0);
                bc->next_idx ^= 1;
            }
        }
        for (int i=0;i<4;i++) {
            if (mpus[i].ready) process_and_send(i);
        }
    }
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  init_bus_ctx();
	for (int i=0;i<4;i++) MPU_Init(mpus[i].i2c, mpus[i].addr);
	poll_loop();
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
