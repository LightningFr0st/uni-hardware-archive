#include "MainFrame.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	mainPanel = new wxPanel(this);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	com_chart = new ChartControl(mainPanel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(640, 480)));
	com_chart->title = "Normalized measured voltage";
	com_chart->values = {};
	com_chart->chartColor = wxRED;

	vbox->Add(com_chart, 1, wxEXPAND | wxALL, 0);

	mainPanel->SetSizer(vbox);

	timer = new wxTimer(this, 1001);
	Bind(wxEVT_TIMER, &MainFrame::OnTimer, this);

	timer->Start(1);

	com_reader.set_callback(std::move([this](uint16_t value)
	{
		com_data.push_back(value / 4095.0);
		if (com_data.size() > 1000)
		{
			com_data.erase(com_data.begin());
		}
	}));
	com_reader.open("COM3");
}

void MainFrame::OnTimer(wxTimerEvent& event)
{
	com_chart->values = com_data;
	com_chart->Refresh();
	com_chart->Update();
}
