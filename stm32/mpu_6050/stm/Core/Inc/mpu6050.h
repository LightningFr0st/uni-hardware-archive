/*
 * mpu6050.h
 *
 *  Created on: Nov 13, 2019
 *      Author: Bulanov Konstantin
 */

#ifndef INC_GY521_H_
#define INC_GY521_H_
#include "spi_utils.h"

#include <stdint.h>

#endif
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
		
	  int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;
	
		double   last_dt;
	
		uint32_t timer;
	
    double KalmanAngleX;
    double KalmanAngleY; // pitch
} MPU6500_t;


uint8_t init_mpu(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin);

void process_mpu_data(MPU6500_t* mpu_state, const uint8_t raw14[14]);

static inline void Kalman_Init(Kalman_t *k)
{
    k->Q_angle   = 0.001f;
    k->Q_bias    = 0.003f;
    k->R_measure = 0.03f;

    k->angle = 0.0f;
    k->bias  = 0.0f;

    k->P[0][0] = 0.0f;
    k->P[0][1] = 0.0f;
    k->P[1][0] = 0.0f;
    k->P[1][1] = 0.0f;
}
