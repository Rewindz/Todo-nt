#pragma once
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/window.h>
#include "TaskUtils.hpp"
#include "Settings.hpp"

namespace ToDont
{
	constexpr long CUSTOM_FRAME_STYLE = wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX);
	class CustomFrame : public wxFrame
	{
	public:
		CustomFrame(wxWindow* parent, wxSize windowSize, Settings& settings);
		~CustomFrame();
	private:
		Settings& m_settings;
		wxBoxSizer* m_sizer;
		TaskTheme m_createdTheme;

		void UpdateSelfTheme(const wxSizer* sizer);

	};
}