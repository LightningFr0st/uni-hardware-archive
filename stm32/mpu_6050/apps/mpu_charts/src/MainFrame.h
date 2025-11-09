#pragma once

#include "ChartControl.h"
#include "com_port_reader.h"

#include <wx/wx.h>
#include <array>
#include <memory>

namespace mpu_6050 
{
	class MpuProcessor;
}

class MainFrame : public wxFrame
{
public:
	explicit MainFrame(const wxString& title);
	~MainFrame() override;

private:
	void OnTimer(wxTimerEvent& event);
	void OnClose(wxCloseEvent& event);

	static int series_index(uint8_t i2c, uint8_t addr) { return (i2c & 1) * 2 + (addr & 1); }

private:
	wxPanel* mainPanel{ nullptr };
	ChartControl* chartX{ nullptr };
	ChartControl* chartY{ nullptr };
	wxTimer* timer{ nullptr };

	::mpu_6050::ComPortReader com_reader;
	std::unique_ptr<::mpu_6050::MpuProcessor> processor;

	wxDECLARE_EVENT_TABLE();
};
