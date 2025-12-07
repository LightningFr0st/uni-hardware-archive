/*
 * mpu6050.c
 *
 *  Created on: Nov 13, 2019
 *      Author: Bulanov Konstantin
 *
 *  Contact information
 *  -------------------
 *
 * e-mail   :  leech001@gmail.com
 */

/*
 * |---------------------------------------------------------------------------------
 * | Copyright (C) Bulanov Konstantin,2021
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |
 * | Kalman filter algorithm used from https://github.com/TKJElectronics/KalmanFilter
 * |---------------------------------------------------------------------------------
 */

#include <math.h>
#include <string.h>

#include "mpu6050.h"

#define RAD_TO_DEG 57.295779513082320876798154814105

#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_2_REG 0x6C

// Setup MPU6050
#define MPU6050_ADDR 0xD0
const uint16_t i2c_timeout = 100;
const double Accel_Z_corrector = 14418.0;

static double Kalman_getAngle(Kalman_t *Kalman, float newAngle, float newRate, float dt)
{
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
};

uint8_t init_mpu(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint8_t check;
    uint8_t Data;

    
    if (mpu_read_register(hspi, cs_port, cs_pin, WHO_AM_I_REG, &check) != HAL_OK)
    {
        return 1;
    }

    if (check == 0x70)
    {
        Data = 0x00;
        if (mpu_write_register(hspi, cs_port, cs_pin, PWR_MGMT_1_REG, Data) != HAL_OK)
            return 1;

        Data = 0x07;
        if (mpu_write_register(hspi, cs_port, cs_pin, SMPLRT_DIV_REG, Data) != HAL_OK)
            return 1;

        Data = 0x00;
        if (mpu_write_register(hspi, cs_port, cs_pin, ACCEL_CONFIG_REG, Data) != HAL_OK)
            return 1;

        Data = 0x00;
        if (mpu_write_register(hspi, cs_port, cs_pin, GYRO_CONFIG_REG, Data) != HAL_OK)
            return 1;
				
				Data = 0x00;
				if (mpu_write_register(hspi, cs_port, cs_pin, PWR_MGMT_2_REG, Data) != HAL_OK)
					return 1;

        return 0;
    }
    return 1;
}

void process_mpu_data(MPU6500_t* mpu_state, const uint8_t raw_data[14])
{
    mpu_state->Accel_X_RAW = (int16_t)(raw_data[0] << 8 | raw_data[1]);
    mpu_state->Accel_Y_RAW = (int16_t)(raw_data[2] << 8 | raw_data[3]);
    mpu_state->Accel_Z_RAW = (int16_t)(raw_data[4] << 8 | raw_data[5]);
    
    mpu_state->Gyro_X_RAW = (int16_t)(raw_data[8] << 8 | raw_data[9]);
    mpu_state->Gyro_Y_RAW = (int16_t)(raw_data[10] << 8 | raw_data[11]);
    mpu_state->Gyro_Z_RAW = (int16_t)(raw_data[12] << 8 | raw_data[13]);

    mpu_state->Ax = mpu_state->Accel_X_RAW / 16384.0;
    mpu_state->Ay = mpu_state->Accel_Y_RAW / 16384.0;
    mpu_state->Az = mpu_state->Accel_Z_RAW / Accel_Z_corrector;
    
    mpu_state->Gx = mpu_state->Gyro_X_RAW / 131.0;
    mpu_state->Gy = mpu_state->Gyro_Y_RAW / 131.0;
    mpu_state->Gz = mpu_state->Gyro_Z_RAW / 131.0;

    // Kalman angle solve
    double dt = (double)(HAL_GetTick() - mpu_state->timer) / 1000;
    mpu_state->timer = HAL_GetTick();
    double roll;
    double roll_sqrt = sqrt(
        mpu_state->Accel_X_RAW * mpu_state->Accel_X_RAW + mpu_state->Accel_Z_RAW * mpu_state->Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll = atan(mpu_state->Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
    double pitch = atan2(-mpu_state->Accel_X_RAW, mpu_state->Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch < -90 && mpu_state->KalmanAngleY > 90) || (pitch > 90 && mpu_state->KalmanAngleY < -90))
    {
        mpu_state->KalmanY.angle = pitch;
        mpu_state->KalmanAngleY = pitch;
    }
    else
    {
        mpu_state->KalmanAngleY = Kalman_getAngle(&mpu_state->KalmanY, pitch, mpu_state->Gy, dt);
    }
    if (fabs(mpu_state->KalmanAngleY) > 90)
        mpu_state->Gx = -mpu_state->Gx;
    mpu_state->KalmanAngleX = Kalman_getAngle(&mpu_state->KalmanX, roll, mpu_state->Gx, dt);
}
