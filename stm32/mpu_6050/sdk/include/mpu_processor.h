#pragma once

#include "mpu_api.h"
#include "mpu_data.h"

#include <array>
#include <functional>
#include <cstdint>
#include <cmath>

namespace mpu_6050
{
	class MPU_API MpuProcessor
	{
	public:
		MpuProcessor(const bool apply_abs = true);

		void set_callback(::std::function<void(MpuOutput const&)>&& cb);
		void feed(MpuPacket const& pkt);

	private:
		struct Kalman {
			double update(double newAngle, double newRate, double dt);

			double Q_angle{ 0.001 };
			double Q_bias{ 0.003 };
			double R_measure{ 0.03 };
			double angle{ 0.0 };
			double bias{ 0.0 };
			double P00{ 0.0 }, P01{ 0.0 }, P10{ 0.0 }, P11{ 0.0 };
		};

		struct SensorState {
			bool inited{ false };
			uint32_t last_tick{ 0 };
			Kalman kx{};
			Kalman ky{};
			double kalX{ 0.0 };
			double kalY{ 0.0 };
		};

		::std::array<SensorState, 4> sensor_state;
		::std::function<void(MpuOutput const&)> callback;
		const bool apply_abs{ true };
	};
}