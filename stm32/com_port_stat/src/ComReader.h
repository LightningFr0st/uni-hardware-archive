#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>

#define NOMINMAX
#include <windows.h>

class ComPortReader 
{

public:

	~ComPortReader();

	bool open(const std::string& portName, DWORD baud = CBR_115200);

	void close();

	void set_callback(std::function<void(uint16_t)>&& cb);

private:
	void reader_thread_proc();

	std::thread reader_thread;
	std::function<void(uint16_t)> callback;

	HANDLE handle{ INVALID_HANDLE_VALUE };
	std::atomic<bool> stop_flag{ false };
};
