#pragma once

#include <stdint.h>

namespace mpu_6050
{
	struct MpuPacket
	{
		double kx{};
		double ky{};

		uint8_t mpu_addr{};
	};
}
