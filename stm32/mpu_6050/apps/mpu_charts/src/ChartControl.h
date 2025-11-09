#pragma once

#include <wx/wx.h>
#include <wx/geometry.h>
#include <wx/font.h>

#include <vector>
#include <deque>
#include <string>
#include <mutex>

class ChartControl : public wxWindow
{
public:
    ChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

    int  add_series(const ::std::string& label, const wxColour& color);
    void push_value(int series_index, double value);

    void set_title(const ::std::string& t);
    void set_capacity(size_t cap);
    void set_manual_range(double ymin, double ymax);
    void clear_manual_range();

private:
    void OnPaint(wxPaintEvent& evt);

    struct Series {
        ::std::string label;
        wxColour color;
        ::std::deque<double> data;
    };

    ::std::vector<Series> series_;
    size_t capacity_{ 1000 };

    bool manual_range_{ false };
    double y_min_{ 0.0 }, y_max_{ 1.0 };

    ::std::string title_;
    wxFont titleFont_;

    const unsigned int gridSegments_{ 11u };
    wxColor gridColor_{ 128,128,128 };
    ::std::mutex mtx_;

    wxDECLARE_EVENT_TABLE();
};
