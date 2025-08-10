#include "TaskButton.hpp"


namespace ToDont
{
	wxBEGIN_EVENT_TABLE(TaskButton, wxControl)
	EVT_PAINT(TaskButton::OnPaint)
	EVT_ENTER_WINDOW(TaskButton::OnMouseEnter)
	EVT_LEAVE_WINDOW(TaskButton::OnMouseLeave)
	EVT_LEFT_DOWN(TaskButton::OnLeftDown)
	EVT_LEFT_UP(TaskButton::OnLeftUp)
	wxEND_EVENT_TABLE()

		
	TaskButton::TaskButton(wxWindow* parent, wxWindowID id, wxSize size, const wxString& label)
		: wxControl(parent, id, wxDefaultPosition, size), m_label(label)
	{
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		SetWindowStyleFlag(wxBORDER_NONE);
		m_theme = GetAvailableThemes().at(0);
	}

	void TaskButton::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		wxSize size = GetClientSize();
		wxColour bg = m_pressed ? m_theme.active: m_hovered ? m_theme.hover : m_theme.bgColor;;

		dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
		dc.SetBrush(wxBrush(bg));
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(0, 0, size.x, size.y);
		dc.SetTextForeground(m_theme.fgColor);
		dc.SetFont(wxFontInfo(11).Bold());
		wxSize textSize = dc.GetTextExtent(m_label);
		wxPoint pos((size.x - textSize.x) / 2, (size.y - textSize.y) / 2);
		dc.DrawText(m_label, pos);
	}

	void TaskButton::OnMouseEnter(wxMouseEvent& event)
	{
		m_hovered = true;
		Refresh();
	}

	void TaskButton::OnMouseLeave(wxMouseEvent& event)
	{
		m_hovered = false;
		m_pressed = false;
		Refresh();
	}

	void TaskButton::OnLeftDown(wxMouseEvent& event)
	{
		m_pressed = true;
		CaptureMouse();
		Refresh();
	}

	void TaskButton::OnLeftUp(wxMouseEvent& event)
	{
		if (HasCapture()) ReleaseMouse();
		m_pressed = false;
		Refresh();

		wxCommandEvent clickEvt(wxEVT_BUTTON, GetId());
		clickEvt.SetEventObject(this);
		ProcessWindowEvent(clickEvt);
	}

	void TaskButton::SetLabel(wxString new_label)
	{
		m_label = new_label;
	}

	wxString TaskButton::GetCurrentLabel()
	{
		return m_label;
	}

}