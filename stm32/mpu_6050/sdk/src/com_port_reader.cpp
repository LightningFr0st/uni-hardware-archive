#include "com_port_reader.h"

#include <string>

namespace mpu_6050
{
	namespace
	{
		static inline int hexval(char c)
		{
			if (c >= '0' && c <= '9') return c - '0';
			if (c >= 'A' && c <= 'F') return c - 'A' + 10;
			if (c >= 'a' && c <= 'f') return c - 'a' + 10;
			return -1;
		}

		bool hex2byte(char hi, char lo, uint8_t& out)
		{
			int h = hexval(hi);
			int l = hexval(lo);
			if (h < 0 || l < 0) return false;
			out = static_cast<uint8_t>((h << 4) | l);
			return true;
		}

		bool parse_hex_line_to_packet(const char* hex34, MpuPacket& out)
		{
			uint8_t b[17];
			for (int i = 0; i < 17; ++i) {
				if (!hex2byte(hex34[2 * i], hex34[2 * i + 1], b[i]))
					return false;
			}

			uint8_t meta = b[0];
			out.i2c = (meta & 0x01u);
			out.addr = ((meta >> 1) & 0x01u);

			auto rd16 = [b](int idxLE)
				{
					return static_cast<int16_t>(static_cast<uint16_t>(b[idxLE]) | (static_cast<uint16_t>(b[idxLE + 1]) << 8));
				};

			out.ax = rd16(1);
			out.ay = rd16(3);
			out.az = rd16(5);

			out.gx = rd16(7);
			out.gy = rd16(9);
			out.gz = rd16(11);

			out.tick = static_cast<uint32_t>(b[13])
				| (static_cast<uint32_t>(b[14]) << 8)
				| (static_cast<uint32_t>(b[15]) << 16)
				| (static_cast<uint32_t>(b[16]) << 24);

			return true;
		}
	}

	ComPortReader::~ComPortReader()
	{
		close();
	}

	bool ComPortReader::open(char const* port_name, DWORD baud)
	{
		close();

		handle = CreateFileA(
			port_name,
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
		reader_thread = ::std::thread(&ComPortReader::reader_thread_proc, this);
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

	void ComPortReader::set_callback(::std::function<void(MpuPacket const&)>&& cb)
	{
		callback = ::std::move(cb);
	}

	void ComPortReader::reader_thread_proc()
	{
		constexpr DWORD CHUNK_SIZE = 256;
		char chunk[CHUNK_SIZE];

		size_t parse_pos = 0;
		::std::string buffer;
		buffer.reserve(4096);

		while (!stop_flag.load(::std::memory_order_relaxed))
		{
			DWORD bytes_read = 0;

			if (!ReadFile(handle, chunk, CHUNK_SIZE, &bytes_read, nullptr)) {
				if (GetLastError() != ERROR_OPERATION_ABORTED) {
					::std::this_thread::sleep_for(::std::chrono::milliseconds(10));
				}
				continue;
			}

			if (bytes_read == 0)
				continue;

			buffer.append(chunk, bytes_read);

			size_t nl_pos = buffer.find('\n', parse_pos);
			while (nl_pos != ::std::string::npos)
			{
				size_t line_start = parse_pos;
				size_t line_end_exclusive = nl_pos;
				if (line_end_exclusive > line_start && buffer[line_end_exclusive - 1] == '\r') {
					--line_end_exclusive;
				}

				size_t line_len = (line_end_exclusive > line_start) ? (line_end_exclusive - line_start) : 0;

				if (line_len == 34 && callback) {
					MpuPacket pkt;
					if (parse_hex_line_to_packet(buffer.data() + line_start, pkt)) {
						callback(pkt);
					}
				}

				parse_pos = nl_pos + 1;
				nl_pos = buffer.find('\n', parse_pos);
			}

			if (parse_pos > 4096) {
				buffer.erase(0, parse_pos);
				parse_pos = 0;
			}
		}
	}
}