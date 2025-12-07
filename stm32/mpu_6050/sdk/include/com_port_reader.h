#pragma once

#include "mpu_api.h"
#include "mpu_data.h"

#include <thread>
#include <atomic>
#include <array>
#include <functional>

#include <Windows.h>

namespace mpu_6050
{
	using namespace std;

	class MPU_API ComPortReader final
	{
	public:
		ComPortReader() = default;

		ComPortReader(ComPortReader const&) = delete;
		ComPortReader(ComPortReader&&) = delete;

		ComPortReader& operator=(ComPortReader const&) = delete;
		ComPortReader& operator=(ComPortReader&&) = delete;

		~ComPortReader();

		bool WINAPI open(char const* port_name, DWORD baud = 115200UL);

		void close();

		void set_callback(function<void(MpuPacket const&)>&& cb);

	private:
		void reader_thread_proc();

		thread reader_thread;
		function<void(MpuPacket const&)> callback;

		HANDLE handle{ INVALID_HANDLE_VALUE };
		atomic<bool> stop_flag{ false };
	};
}

