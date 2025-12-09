#pragma once

#include "ChartControl.h"
#include "com_port_reader.h"

#include <wx/wx.h>
#include <array>
#include <memory>
#include <atomic>

class MainFrame : public wxFrame
{
public:
	explicit MainFrame(const wxString& title);
	~MainFrame() override;

private:
	void OnTimer(wxTimerEvent& event);
	void OnClose(wxCloseEvent& event);

	void OnRateTimer(wxTimerEvent& event);

	static int series_index(uint8_t i2c, uint8_t addr) { return (i2c & 1) * 2 + (addr & 1); }

private:
	wxPanel* mainPanel{ nullptr };
	ChartControl* chartX{ nullptr };
	ChartControl* chartY{ nullptr };
	wxTimer* timer{ nullptr };

	wxTimer* rateTimer{ nullptr };
	wxStaticText* rateLabel{ nullptr };

	std::atomic<uint32_t> packetsCounter{ 0 };

	::mpu_6050::ComPortReader com_reader;

	wxDECLARE_EVENT_TABLE();
};
