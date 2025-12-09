#include "MainFrame.h"

#include <wx/sizer.h>
#include <wx/settings.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(1001, MainFrame::OnTimer)
EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

static inline double unsigned_tilt(double a_deg)
{
	double x = ::std::fmod(a_deg, 360.0);
	if (x < 0.0) x += 360.0;
	if (x > 180.0) x = 360.0 - x;
	return x;
}

MainFrame::MainFrame(wxString const& title)
	: wxFrame(nullptr, wxID_ANY, title)
{
	using namespace mpu_6050;

	mainPanel = new wxPanel(this);

	auto* vbox = new wxBoxSizer(wxVERTICAL);

	rateLabel = new wxStaticText(mainPanel, wxID_ANY, "Rate: 0 packets/s");
	vbox->Add(rateLabel, 0, wxLEFT | wxTOP | wxRIGHT, this->FromDIP(4));

	chartX = new ChartControl(mainPanel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(900, 260)));
	chartY = new ChartControl(mainPanel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(900, 260)));

	chartX->set_title("Kalman Angle X");
	chartY->set_title("Kalman Angle Y");

	wxColour const colors[5] = { *wxRED, *wxBLUE, *wxGREEN, *wxCYAN, *wxYELLOW };
	for (int i = 0; i < 5; ++i) {
		string const label = "MPU" + to_string(i);
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

	com_reader.set_callback([this](MpuPacket const& pkt)
		{
			packetsCounter.fetch_add(1, std::memory_order_relaxed);

			chartX->CallAfter([this, pkt] { chartX->push_value(pkt.mpu_addr, unsigned_tilt(pkt.kx)); });
			chartY->CallAfter([this, pkt] { chartY->push_value(pkt.mpu_addr, unsigned_tilt(pkt.ky)); });
		});
	com_reader.open("COM5");

	timer = new wxTimer(this, 1001);
	timer->Start(16);

	rateTimer = new wxTimer(this, 1002);
	rateTimer->Start(1000);
	Bind(wxEVT_TIMER, &MainFrame::OnRateTimer, this, rateTimer->GetId());
}

MainFrame::~MainFrame()
{
	if (timer) { timer->Stop(); }
	if (rateTimer) { rateTimer->Stop(); }
	com_reader.close();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	if (timer) { timer->Stop(); }
	if (rateTimer) { rateTimer->Stop(); }
	com_reader.close();
	Destroy();
}

void MainFrame::OnRateTimer(wxTimerEvent&)
{
	uint32_t packets = packetsCounter.exchange(0, std::memory_order_relaxed);

	rateLabel->SetLabel(wxString::Format("Rate: %u packets/s", packets));
}

void MainFrame::OnTimer(wxTimerEvent&)
{
	chartX->Refresh(false);
	chartY->Refresh(false);
	chartX->Update();
	chartY->Update();
}
