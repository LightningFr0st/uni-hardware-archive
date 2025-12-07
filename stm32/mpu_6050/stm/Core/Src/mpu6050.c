#include <math.h>

#include "mpu6050.h"

#define RAD_TO_DEG 57.295779513082320876798154814105
#define ACCEL_Z_CORRECTOR 14418.0;

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

void Kalman_Init(Kalman_t *k)
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

void process_mpu_data(MPU6500_t* mpu_state, const uint8_t raw_data[14])
{
    int16_t Accel_X_RAW = (int16_t)(raw_data[0] << 8 | raw_data[1]);
    int16_t Accel_Y_RAW = (int16_t)(raw_data[2] << 8 | raw_data[3]);
    int16_t Accel_Z_RAW = (int16_t)(raw_data[4] << 8 | raw_data[5]);
    
    int16_t Gyro_X_RAW = (int16_t)(raw_data[8] << 8 | raw_data[9]);
    int16_t Gyro_Y_RAW = (int16_t)(raw_data[10] << 8 | raw_data[11]);
    int16_t Gyro_Z_RAW = (int16_t)(raw_data[12] << 8 | raw_data[13]);

    double Ax = Accel_X_RAW / 16384.0;
    double Ay = Accel_Y_RAW / 16384.0;
    double Az = Accel_Z_RAW / ACCEL_Z_CORRECTOR;
    
    double Gx = Gyro_X_RAW / 131.0;
    double Gy = Gyro_Y_RAW / 131.0;
    double Gz = Gyro_Z_RAW / 131.0;

    
    double dt = (double)(HAL_GetTick() - mpu_state->timer) / 1000;
    mpu_state->timer = HAL_GetTick();
    
		double roll;
    double roll_sqrt = sqrt(Accel_X_RAW * Accel_X_RAW + Accel_Z_RAW * Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll = atan(Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
		
    double pitch = atan2(-Accel_X_RAW, Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch < -90 && mpu_state->KalmanAngleY > 90) || (pitch > 90 && mpu_state->KalmanAngleY < -90))
    {
        mpu_state->KalmanY.angle = pitch;
        mpu_state->KalmanAngleY = pitch;
    }
    else
    {
        mpu_state->KalmanAngleY = Kalman_getAngle(&mpu_state->KalmanY, pitch, Gy, dt);
    }
		
    if (fabs(mpu_state->KalmanAngleY) > 90)
        Gx = -Gx;
    mpu_state->KalmanAngleX = Kalman_getAngle(&mpu_state->KalmanX, roll, Gx, dt);
}
