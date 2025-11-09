#include "mpu_processor.h"

#define _USE_MATH_DEFINES
#include "math.h"

namespace mpu_6050
{
	namespace 
	{
		static constexpr double RAD_TO_DEG = 180.0 / M_PI;
		static constexpr double ACC_SCALE = 16384.0;
		static constexpr double GYRO_SCALE = 131.0;
		static constexpr double ACC_Z_CORR = 14418.0;

		static inline double unsigned_tilt(double a_deg)
		{
			double x = ::std::fmod(a_deg, 360.0);
			if (x < 0.0) x += 360.0;
			if (x > 180.0) x = 360.0 - x;
			return x;
		}
	}

	double MpuProcessor::Kalman::update(double newAngle, double newRate, double dt)
	{
		double rate = newRate - bias;
		angle += dt * rate;

		P00 += dt * (dt * P11 - P01 - P10 + Q_angle);
		P01 -= dt * P11;
		P10 -= dt * P11;
		P11 += Q_bias * dt;

		double S = P00 + R_measure;
		double K0 = P00 / S;
		double K1 = P10 / S;

		double y = newAngle - angle;
		angle += K0 * y;
		bias += K1 * y;

		double P00t = P00, P01t = P01;
		P00 -= K0 * P00t;
		P01 -= K0 * P01t;
		P10 -= K1 * P00t;
		P11 -= K1 * P01t;

		return angle;
	}

	MpuProcessor::MpuProcessor(const bool _apply_abs)
		:apply_abs(_apply_abs)
	{
	}

	void MpuProcessor::set_callback(::std::function<void(MpuOutput const&)>&& cb)
	{
		callback = ::std::move(cb);
	}

	void MpuProcessor::feed(MpuPacket const& pkt)
	{
		int const id = (pkt.i2c & 1) * 2 + (pkt.addr & 1);

		SensorState& st = sensor_state[id];

		double const Ax = pkt.ax / ACC_SCALE;
		double const Ay = pkt.ay / ACC_SCALE;
		double const Az = pkt.az / ACC_Z_CORR;

		double Gx = pkt.gx / GYRO_SCALE;
		double Gy = pkt.gy / GYRO_SCALE;
		double Gz = pkt.gz / GYRO_SCALE;

		auto calc_roll_pitch = [](MpuPacket const& pkt) -> ::std::pair<double, double>
			{
				double roll_sqrt = ::std::sqrt(::std::pow(pkt.ax, 2) + ::std::pow(pkt.az, 2));
				double roll = (roll_sqrt != 0.0) ? ::std::atan(double(pkt.ay) / roll_sqrt) * RAD_TO_DEG : 0.0;
				double pitch = ::std::atan2(-double(pkt.ax), double(pkt.az)) * RAD_TO_DEG;

				return { roll, pitch };
			};

		double dt = 0.0;
		if (!st.inited)
		{
			st.inited = true;
			st.last_tick = pkt.tick;

			::std::pair<double, double> roll_pitch = calc_roll_pitch(pkt);

			st.kx.angle = roll_pitch.first;
			st.ky.angle = roll_pitch.second;
			st.kalX = roll_pitch.first;
			st.kalY = roll_pitch.second;
		}
		else
		{
			uint32_t dt_ms = pkt.tick - st.last_tick;
			st.last_tick = pkt.tick;
			dt = double(dt_ms) / 1000.0;
			if (dt <= 0.0) dt = 1e-3;
		}

		::std::pair<double, double> roll_pitch = calc_roll_pitch(pkt);
		double roll = roll_pitch.first;
		double pitch = roll_pitch.second;

		if ((pitch < -90.0 && st.kalY > 90.0) || (pitch > 90.0 && st.kalY < -90.0))
		{
			st.ky.angle = pitch;
			st.kalY = pitch;
		}
		else
		{
			st.kalY = st.ky.update(pitch, Gy, dt);
		}

		if (::std::fabs(st.kalY) > 90.0) Gx = -Gx;

		st.kalX = st.kx.update(roll, Gx, dt);

		callback(
			{
				.KalmanAngleX = apply_abs ? unsigned_tilt(st.kalX) : st.kalX,
				.KalmanAngleY = apply_abs ? unsigned_tilt(st.kalY) : st.kalY,
				.Ax = Ax,
				.Ay = Ay,
				.Az = Az,
				.Gx = Gx,
				.Gy = Gy,
				.Gz = Gz,
				.tick = pkt.tick,
				.i2c = pkt.i2c,
				.addr = pkt.addr,
			}
			);
	}
}
