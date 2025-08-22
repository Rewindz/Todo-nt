#include "TaskCheck.hpp"

#include <algorithm>

#include "MainFrame.hpp"
#include "utils.hpp"

namespace ToDont
{

	TaskCheck::TaskCheck(const Settings& settings, wxWindow* parent, wxWindowID id, bool isChecked)
		: wxControl(),
		m_checked(isChecked), m_settings(settings)
	{


		SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

		Create(parent, id, wxDefaultPosition, FromDIP(wxSize(24, 24)), wxBORDER_NONE);

		Bind(wxEVT_PAINT, &TaskCheck::OnPaint, this);
		Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& evt)
			{
				this->m_hovered = true;
				Refresh();
				evt.Skip();
			});
		Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& evt)
			{
				this->m_hovered = false;
				Refresh();
				evt.Skip();
			});
		Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& evt)
			{
				this->m_active = true;
				Refresh();
			});
		Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& evt)
			{
				this->m_active = false;
				this->m_checked = !this->m_checked;
				Refresh();

				wxCommandEvent e(wxEVT_CHECKBOX, GetId());
				e.SetInt(m_checked);
				e.SetEventObject(this);
				wxPostEvent(this, e);
			});

		const int pad = FromDIP(4);
		const int box = FromDIP(18);
		SetInitialSize(wxSize(box + 2 * pad, box + 2 * pad));
		SetMaxSize(wxSize(box + 2 * pad, box + 2 * pad));

	}


	void TaskCheck::OnPaint(wxPaintEvent& evt){
		wxPaintDC dc(this);
		const auto sz = GetClientSize();
		const auto theme = m_settings.GetTheme();

		//dc.SetPen(wxColour(255, 0, 0));
		auto penColour = (m_active ? theme.active : m_hovered ? theme.hover : theme.fgColor);
		auto brushColour = utils::wxColourLerp(theme.bgColor, theme.fgColor, .2f);
		dc.SetBrush((m_checked ? brushColour : *wxTRANSPARENT_BRUSH));
		dc.SetPen(penColour);
		auto rad = std::min(sz.x, sz.y) / 2 - 2;

		dc.DrawCircle(sz.x / 2, sz.y / 2, rad);
	}
}