#pragma once

#include "mpu_api.h"
#include "mpu_data.h"

#include <thread>
#include <atomic>
#include <functional>

#include <Windows.h>

namespace mpu_6050 
{
	class MPU_API ComPortReader final
	{
	public:

		~ComPortReader();

		bool open(char const* port_name, DWORD baud = 115200UL);

		void close();

		void set_callback(::std::function<void(MpuPacket const&)>&& cb);

	private:
		void reader_thread_proc();

		::std::thread reader_thread;
		::std::function<void(MpuPacket const&)> callback;

		HANDLE handle{ INVALID_HANDLE_VALUE };
		::std::atomic<bool> stop_flag{ false };
	};
}

