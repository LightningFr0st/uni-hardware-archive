#include "MainFrame.h"
#include "mpu_processor.h"

#include <wx/sizer.h>
#include <wx/settings.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(1001, MainFrame::OnTimer)
EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxString const& title)
	: wxFrame(nullptr, wxID_ANY, title)
{
	mainPanel = new wxPanel(this);

	auto* vbox = new wxBoxSizer(wxVERTICAL);

	chartX = new ChartControl(mainPanel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(900, 260)));
	chartY = new ChartControl(mainPanel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(900, 260)));

	chartX->set_title("Kalman Angle X");
	chartY->set_title("Kalman Angle Y");

	wxColour const colors[4] = { *wxRED, *wxBLUE, *wxGREEN, *wxCYAN };
	for (int i = 0; i < 4; ++i) {
		int const i2c = i / 2;
		int const addr = i % 2;
		::std::string const label = "I" + ::std::to_string(i2c) + "-A" + ::std::to_string(addr);
		chartX->add_series(label, colors[i]);
		chartY->add_series(label, colors[i]);
	}

	chartX->set_capacity(100);
	chartY->set_capacity(100);

	chartX->set_manual_range(0.0, 190.0);
	chartY->set_manual_range(0.0, 190.0);

	vbox->Add(chartX, 1, wxEXPAND | wxALL, this->FromDIP(4));
	vbox->Add(chartY, 1, wxEXPAND | wxALL, this->FromDIP(4));
	mainPanel->SetSizer(vbox);

	processor = ::std::make_unique<::mpu_6050::MpuProcessor>();
	processor->set_callback([this](::mpu_6050::MpuOutput const& o)
		{
			int const idx = series_index(o.i2c, o.addr);
			chartX->CallAfter([this, idx, o] { chartX->push_value(idx, o.KalmanAngleX); });
			chartY->CallAfter([this, idx, o] { chartY->push_value(idx, o.KalmanAngleY); });
		});
	com_reader.set_callback([this](::mpu_6050::MpuPacket const& pkt) {
		processor->feed(pkt);
		});
	com_reader.open("COM3");

	timer = new wxTimer(this, 1001);
	timer->Start(16);
}

MainFrame::~MainFrame()
{
	if (timer) { timer->Stop(); }
	com_reader.close();
	processor.reset();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	if (timer) { timer->Stop(); }
	com_reader.close();
	processor.reset();

	Destroy();
}

void MainFrame::OnTimer(wxTimerEvent&)
{
	chartX->Refresh(false);
	chartY->Refresh(false);
	chartX->Update();
	chartY->Update();
}
