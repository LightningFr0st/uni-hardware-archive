#include "ComReader.h"

#include <vector>
#include <iostream>
#include <type_traits>

ComPortReader::~ComPortReader()
{
	close();
}

bool ComPortReader::open(const std::string& portName, DWORD baud)
{
	close();

	handle = CreateFileA(
		portName.c_str(),
		GENERIC_READ,
		0ul,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DCB dcb = { 0 };

	if (!GetCommState(handle, &dcb))
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
		return false;
	}
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(handle, &dcb))
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
		return false;
	}

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = MAXWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 100;

	SetCommTimeouts(handle, &timeouts);

	PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

	stop_flag = false;
	reader_thread = std::thread(&ComPortReader::reader_thread_proc, this);
	return true;
}

void ComPortReader::close()
{
	stop_flag = true;
	if (reader_thread.joinable())
	{
		reader_thread.join();
	}
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

void ComPortReader::set_callback(std::function<void(uint16_t)>&& cb)
{
	callback = std::move(cb);
}

void ComPortReader::reader_thread_proc()
{
	constexpr uint32_t CHUNK_SIZE{ 256ul };
	char chunk[CHUNK_SIZE];

	size_t parse_pos{ 0ull };

	std::string buffer;

	while (stop_flag.load(std::memory_order_relaxed) == false)
	{
		DWORD bytes_read{ 0ul };

		if (!ReadFile(handle, chunk, CHUNK_SIZE, &bytes_read, nullptr)) {
			if (GetLastError() != ERROR_OPERATION_ABORTED) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10ll));
			}
			continue;
		}

		if (bytes_read > 0ul) {
			buffer.append(chunk, bytes_read);

			size_t nl_pos{ buffer.find('\n', parse_pos) };

			while (nl_pos != std::string::npos)
			{
				const std::string numberStr{ buffer.substr(parse_pos, nl_pos - parse_pos - 1) };
				const uint16_t value{ static_cast<uint16_t>(std::stoul(numberStr)) };

				callback(value);

				parse_pos = nl_pos + 1;
				nl_pos = buffer.find('\n', parse_pos);
			}

			if (parse_pos > 4096ull) {
				buffer = buffer.substr(parse_pos);
				parse_pos = 0ull;
			}
		}
	}
}
