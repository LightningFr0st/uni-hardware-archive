#include <algorithm>
#include <cmath>

#include "ChartControl.h"

#include <wx/settings.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

ChartControl::ChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE)
{
	titleFont = wxFont(wxNORMAL_FONT->GetPointSize() * 2.0, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	SetBackgroundStyle(wxBG_STYLE_PAINT);
	Bind(wxEVT_PAINT, &ChartControl::OnPaint, this);
}

void ChartControl::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	if (gc && values.size() > 1)
	{
		gc->SetFont(titleFont, wxSystemSettings::GetAppearance().IsDark() ? *wxWHITE : *wxBLACK);

		double text_w, text_h;
		gc->GetTextExtent(title, &text_w, &text_h);

		int width{ GetSize().GetWidth() };
		int height{ GetSize().GetHeight() };
		const int MinimumMargin{ FromDIP(10) };
		const double marginX{ width / 8.0 };
		const double marginTop = std::max(height / 8.0, MinimumMargin * 2.0 + text_h);
		const double marginBottom = height / 8.0;;

		wxRect2DDouble fullArea{ 0.0, 0.0, static_cast<wxDouble>(width), static_cast<wxDouble>(height) };
		fullArea.Inset(marginX, marginTop, marginX, marginBottom);

		gc->DrawText(title, (width - text_w) / 2.0, (marginTop - text_h) / 2.0);

		double left{ fullArea.GetLeft() };

		wxAffineMatrix2D normalizedToChartArea{};
		normalizedToChartArea.Translate(left, fullArea.GetTop());
		normalizedToChartArea.Scale(fullArea.m_width, fullArea.m_height);

		wxAffineMatrix2D normalizedToValue{};

		normalizedToValue.Translate(0, rangeHigh);
		normalizedToValue.Scale(1, -1);
		normalizedToValue.Scale(static_cast<double>(values.size() - 1), 1.0);

		gc->SetPen(wxPen(wxColor(128, 128, 128)));
		gc->SetFont(*wxNORMAL_FONT, wxSystemSettings::GetAppearance().IsDark() ? *wxWHITE : *wxBLACK);

		for (unsigned int i = 0u; i < segmentCount; ++i)
		{
			double normalizedLineY = static_cast<double>(i) / (segmentCount - 1);

			wxPoint2DDouble lineStartPoint = normalizedToChartArea.TransformPoint({ 0, normalizedLineY });
			wxPoint2DDouble lineEndPoint = normalizedToChartArea.TransformPoint({ 1, normalizedLineY });

			wxPoint2DDouble linePoints[] = { lineStartPoint, lineEndPoint };

			gc->StrokeLines(2, linePoints);

			double ValueAtLineY = normalizedToValue.TransformPoint({ 0, normalizedLineY }).m_y;

			auto text = wxString::Format("%.2f", ValueAtLineY);
			text = wxControl::Ellipsize(text, dc, wxELLIPSIZE_MIDDLE, left - MinimumMargin);

			double tw, th;
			gc->GetTextExtent(text, &tw, &th);
			gc->DrawText(text, left - MinimumMargin - tw, lineStartPoint.m_y - th / 2.0);
		}

		wxPoint2DDouble leftHLinePoints[] = {
			normalizedToChartArea.TransformPoint({0, 0}),
			normalizedToChartArea.TransformPoint({0, 1}) };

		wxPoint2DDouble rightHLinePoints[] = {
			normalizedToChartArea.TransformPoint({1, 0}),
			normalizedToChartArea.TransformPoint({1, 1}) };

		gc->StrokeLines(2, leftHLinePoints);
		gc->StrokeLines(2, rightHLinePoints);

		wxAffineMatrix2D valueToNormalized = normalizedToValue;
		valueToNormalized.Invert();
		wxAffineMatrix2D valueToChartArea = normalizedToChartArea;
		valueToChartArea.Concat(valueToNormalized);

		for (int i = 0; i < values.size(); i++)
		{
			pointArray[i] = valueToChartArea.TransformPoint(wxPoint2DDouble{ static_cast<double>(i), values[i] });
		}

		if (chartColor)
		{
			gc->SetPen(wxPen(*chartColor, 3));
		}
		else
		{
			gc->SetPen(wxPen(wxSystemSettings::GetAppearance().IsDark() ? *wxCYAN : *wxBLUE, 3));
		}

		gc->StrokeLines(values.size(), pointArray.data());
	}
}