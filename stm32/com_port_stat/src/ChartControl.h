#pragma once

#include <wx/wx.h>
#include <vector>
#include <string>
#include <tuple>
#include <array>

#include <wx/geometry.h>
#include <wx/font.h>

class ChartControl : public wxWindow
{
public:
    ChartControl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);

    std::vector<double> values;
    std::string title;

    const wxColor *chartColor = nullptr;

private:

    void OnPaint(wxPaintEvent &evt);
  
    const unsigned int segmentCount{ 11u };
    const double rangeLow{ 0.0 };
    const double rangeHigh{ 1.0 };

    std::array<wxPoint2DDouble, 1001> pointArray;

    wxFont titleFont;
};