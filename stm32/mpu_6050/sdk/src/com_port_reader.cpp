#include "com_port_reader.h"

#include <string>
#include <charconv>

namespace mpu_6050
{
	namespace
	{
		bool parse_text_line_to_packet(string_view line, MpuPacket& pkt)
		{
			if (line.size() < 2 || line.front() != '#')
				return false;

			line.remove_prefix(1);

			const auto h1 = line.find('#');
			if (h1 == string_view::npos)
				return false;

			const auto h2 = line.find('#', h1 + 1);
			if (h2 == string_view::npos)
				return false;

			string_view id_sv = line.substr(0, h1);
			string_view kx_sv = line.substr(h1 + 1, h2 - (h1 + 1));
			string_view ky_sv = line.substr(h2 + 1);

			if (!ky_sv.empty() && ky_sv.back() == '\r')
				ky_sv.remove_suffix(1);

			int idx = 0;
			float kx = 0.0f;
			float ky = 0.0f;

			{
				const char* first = id_sv.data();
				const char* last = id_sv.data() + id_sv.size();
				auto res = from_chars(first, last, idx);
				if (res.ec != errc{} || res.ptr != last)
					return false;
			}

			try
			{
				kx = stof(string(kx_sv));
				ky = stof(string(ky_sv));
			}
			catch (...) {
				return false;
			}

			pkt.mpu_addr = idx;
			pkt.kx = kx;
			pkt.ky = ky;

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
		reader_thread = thread(&ComPortReader::reader_thread_proc, this);
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

	void ComPortReader::set_callback(function<void(MpuPacket const&)>&& cb)
	{
		callback = move(cb);
	}

	void ComPortReader::reader_thread_proc()
	{
		constexpr DWORD CHUNK_SIZE = 256;
		char chunk[CHUNK_SIZE];

		size_t parse_pos = 0;
		string buffer;
		buffer.reserve(4096);

		while (!stop_flag.load(memory_order_relaxed))
		{
			DWORD bytes_read = 0;

			if (!ReadFile(handle, chunk, CHUNK_SIZE, &bytes_read, nullptr)) {
				if (GetLastError() != ERROR_OPERATION_ABORTED) {
					this_thread::sleep_for(chrono::milliseconds(10));
				}
				continue;
			}

			if (bytes_read == 0)
				continue;

			buffer.append(chunk, bytes_read);

			size_t nl_pos = buffer.find('\n', parse_pos);
			while (nl_pos != string::npos)
			{
				size_t line_start = parse_pos;
				size_t line_end_exclusive = nl_pos;
				if (line_end_exclusive > line_start && buffer[line_end_exclusive - 1] == '\r') {
					--line_end_exclusive;
				}

				size_t line_len = (line_end_exclusive > line_start) ? (line_end_exclusive - line_start) : 0;

				if (line_len > 0 && callback) {

					MpuPacket pkt;
					string_view line_view(buffer.data() + line_start, line_len);

					if (parse_text_line_to_packet(line_view, pkt)) {
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