#pragma once

#include <stdint.h>

namespace mpu_6050
{
	struct MpuPacket
	{
		uint32_t tick{ 0 };
		int16_t ax{ 0 };
		int16_t ay{ 0 };
		int16_t az{ 0 };
		int16_t gx{ 0 };
		int16_t gy{ 0 };
		int16_t gz{ 0 };
		uint8_t i2c{ 0 };
		uint8_t addr{ 0 };
	};

	struct MpuOutput
	{
		double KalmanAngleX{ 0.0 };
		double KalmanAngleY{ 0.0 };
		double Ax{ 0.0 };
		double Ay{ 0.0 };
		double Az{ 0.0 };
		double Gx{ 0.0 };
		double Gy{ 0.0 };
		double Gz{ 0.0 };
		uint32_t tick{ 0U };
		uint8_t i2c{ 0 };
		uint8_t addr{ 0 };
	};
}
