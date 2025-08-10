#pragma once

#include <wx/wx.h>
#include "TaskUtils.hpp"

namespace ToDont
{
	class TaskButton : public wxControl
	{
	public:
		TaskButton(wxWindow* parent, wxWindowID id, wxSize size, const wxString& label);

		void SetLabel(wxString new_label);
		wxString GetCurrentLabel();
		void SetTheme(TaskTheme theme) { m_theme = theme; }
	private:
		void OnPaint(wxPaintEvent& event);
		void OnMouseEnter(wxMouseEvent& event);
		void OnMouseLeave(wxMouseEvent& event);
		void OnLeftDown(wxMouseEvent& event);
		void OnLeftUp(wxMouseEvent& event);

		bool m_hovered = false;
		bool m_pressed = false;
		TaskTheme m_theme;
		wxString m_label;

		wxDECLARE_EVENT_TABLE();

	};

}