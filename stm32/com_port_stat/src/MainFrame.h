#pragma once

#include "ChartControl.h"
#include "ComReader.h"

#include <wx/wx.h>
#include <wx/grid.h>

#include <vector>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void OnTimer(wxTimerEvent& event);

	ComPortReader com_reader;

	std::vector<double> com_data;

	wxPanel* mainPanel{ nullptr };
	ChartControl* com_chart{ nullptr };
	wxTimer* timer{ nullptr };
};