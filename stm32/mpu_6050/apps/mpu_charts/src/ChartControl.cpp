#include "ChartControl.h"
#include <wx/settings.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <algorithm>
#include <limits>

wxBEGIN_EVENT_TABLE(ChartControl, wxWindow)
EVT_PAINT(ChartControl::OnPaint)
wxEND_EVENT_TABLE()

ChartControl::ChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE)
{
    titleFont_ = wxFont(wxNORMAL_FONT->GetPointSize() * 2.0,
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

int ChartControl::add_series(const ::std::string& label, const wxColour& color)
{
    ::std::lock_guard<::std::mutex> lock(mtx_);
    series_.push_back(Series{ label, color, {} });
    return static_cast<int>(series_.size() - 1);
}

void ChartControl::push_value(int series_index, double value)
{
    if (series_index < 0) return;
    ::std::lock_guard<::std::mutex> lock(mtx_);
    if (series_index >= static_cast<int>(series_.size())) return;

    auto& dq = series_[series_index].data;
    dq.push_back(value);
    if (dq.size() > capacity_) dq.pop_front();

    Refresh(false);
}

void ChartControl::set_title(const ::std::string& t)
{
    ::std::lock_guard<::std::mutex> lock(mtx_);
    title_ = t;
}

void ChartControl::set_capacity(size_t cap)
{
    ::std::lock_guard<::std::mutex> lock(mtx_);
    capacity_ = ::std::max<size_t>(2, cap);
    for (auto& s : series_) {
        while (s.data.size() > capacity_) s.data.pop_front();
    }
}

void ChartControl::set_manual_range(double ymin, double ymax)
{
    ::std::lock_guard<::std::mutex> lock(mtx_);
    manual_range_ = true;
    y_min_ = ymin; y_max_ = ymax;
}

void ChartControl::clear_manual_range()
{
    ::std::lock_guard<::std::mutex> lock(mtx_);
    manual_range_ = false;
}

void ChartControl::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    auto gc(::std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(dc)));
    if (!gc) return;

    ::std::vector<Series> local;
    ::std::string title;
    bool manual = false; double ylo = 0, yhi = 1;
    {
        ::std::lock_guard<::std::mutex> lock(mtx_);
        local = series_;
        title = title_;
        manual = manual_range_;
        ylo = y_min_; yhi = y_max_;
    }

    size_t maxN = 0;
    for (auto& s : local) maxN = ::std::max(maxN, s.data.size());
    if (maxN < 2) return;

    const bool dark = wxSystemSettings::GetAppearance().IsDark();
    gc->SetFont(titleFont_, dark ? *wxWHITE : *wxBLACK);

    double text_w = 0, text_h = 0;
    gc->GetTextExtent(title, &text_w, &text_h);

    const int W = GetSize().GetWidth();
    const int H = GetSize().GetHeight();
    const int minMargin = FromDIP(10);
    const double marginX = W / 10.0;
    const double marginTop = ::std::max(H / 12.0, minMargin * 2.0 + text_h);
    const double marginBottom = H / 10.0;

    wxRect2DDouble plot{ 0,0,(wxDouble)W,(wxDouble)H };
    plot.Inset(marginX, marginTop, marginX, marginBottom);

    gc->DrawText(title, (W - text_w) / 2.0, (marginTop - text_h) / 2.0);
    
    if (!manual) {
        double mn = ::std::numeric_limits<double>::infinity();
        double mx = -::std::numeric_limits<double>::infinity();
        for (auto& s : local) for (double v : s.data) { mn = ::std::min(mn, v); mx = ::std::max(mx, v); }
        if (!::std::isfinite(mn) || !::std::isfinite(mx) || mn == mx) { mn = 0; mx = 1; }
        const double pad = 0.05 * (mx - mn + 1e-12);
        ylo = mn - pad; yhi = mx + pad;
    }

    const double sx = (maxN > 1) ? (plot.m_width / (double)(maxN - 1)) : 0.0;
    const double sy = (yhi != ylo) ? (plot.m_height / (yhi - ylo)) : 1.0;

    auto to_px = [&](size_t i, double val)->wxPoint2DDouble {
        const double x = plot.GetLeft() + (double)i * sx;
        const double y = plot.GetTop() + (yhi - val) * sy;
        return { x, y };
        };

    gc->SetPen(wxPen(gridColor_));
    gc->SetFont(*wxNORMAL_FONT, dark ? *wxWHITE : *wxBLACK);

    for (unsigned i = 0; i < gridSegments_; ++i) {
        const double ny = (double)i / (gridSegments_ - 1);
        const double gy = plot.GetTop() + (1.0 - ny) * plot.m_height;
        wxPoint2DDouble ln[] = { {plot.GetLeft(), gy}, {plot.GetRight(), gy} };
        gc->StrokeLines(2, ln);

        const double valY = ylo + ny * (yhi - ylo);
        wxString t = wxString::Format("%.2f", valY);
        double tw = 0, th = 0; gc->GetTextExtent(t, &tw, &th);
        gc->DrawText(t, plot.GetLeft() - minMargin - tw, gy - th / 2.0);
    }

    {
        wxPoint2DDouble ln[] = { {plot.GetLeft(), plot.GetTop()}, {plot.GetLeft(), plot.GetBottom()} };
        gc->StrokeLines(2, ln);
    }
    {
        wxPoint2DDouble ln[] = { {plot.GetRight(), plot.GetTop()}, {plot.GetRight(), plot.GetBottom()} };
        gc->StrokeLines(2, ln);
    }

    for (auto& s : local) {
        const size_t N = s.data.size();
        if (N < 2) continue;

        gc->SetPen(wxPen(s.color, 2));

        ::std::vector<wxPoint2DDouble> pts;
        pts.reserve(N);
        for (size_t i = 0; i < N; ++i) {
            pts.push_back(to_px(i, s.data[i]));
        }

        gc->StrokeLines((int)pts.size(), pts.data());
    }

    const double legPad = FromDIP(6);
    const double dot = FromDIP(12);
    double x = plot.GetRight() - FromDIP(160);
    double y = plot.GetTop() + FromDIP(8);

    for (auto& s : local) {
        gc->SetPen(wxPen(s.color, 3));
        wxPoint2DDouble ln[] = { {x, y + dot / 2.0}, {x + dot, y + dot / 2.0} };
        gc->StrokeLines(2, ln);
        gc->SetFont(*wxNORMAL_FONT, dark ? *wxWHITE : *wxBLACK);
        gc->DrawText(s.label, x + dot + legPad, y);
        double tw = 0, th = 0; gc->GetTextExtent(s.label, &tw, &th);
        y += th + legPad;
    }
}
