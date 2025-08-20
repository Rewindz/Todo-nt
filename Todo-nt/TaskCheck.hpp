#pragma once
#include <wx/control.h>
#include <wx/window.h>
#include <wx/dcclient.h>

#include <stdexcept>

#include "Settings.hpp"


namespace ToDont
{

	class TaskCheck : public wxControl
	{
	public:
		TaskCheck(const Settings& settings, wxWindow* parent, wxWindowID id, bool isChecked = false);

		bool Checked() const { return m_checked; }
		void SetChecked(bool b) { m_checked = b; }

		~TaskCheck() = default;

	private:
		bool m_checked;
		bool m_hovered = false;
		bool m_active = false;
		const Settings& m_settings;


		void OnPaint(wxPaintEvent& evt);


	};
}