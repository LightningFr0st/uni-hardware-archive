

#ifndef INC_GY521_H_

#define INC_GY521_H_

#include "spi_utils.h"

#include <stdint.h>

// MPU registers

#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define PWR_MGMT_2_REG 0x6C
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43

// Kalman structure
typedef struct
{
    float Q_angle;
    float Q_bias;
    float R_measure;
    float angle;
    float bias;
    float P[2][2];
} Kalman_t;

// MPU6050 structure
typedef struct
{
		Kalman_t KalmanX;
		Kalman_t KalmanY;
		
		uint32_t timer;
	
    double KalmanAngleX;
    double KalmanAngleY;
} MPU6500_t;


uint8_t init_mpu(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin);

void process_mpu_data(MPU6500_t* mpu_state, const uint8_t raw14[14]);

void Kalman_Init(Kalman_t *k);

#endif