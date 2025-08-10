#pragma once

#include <wx/frame.h>
#include <wx/window.h>
#include <nlohmann/json.hpp>
#include "Settings.hpp"

namespace ToDont
{
	constexpr long SETTINGS_FRAME_STYLE = wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX | wxRESIZE_BORDER);
	class SettingsFrame : public wxFrame
	{
	public:
		SettingsFrame(wxWindow* parent, wxSize windowSize, bool* settings_open, Settings& settings);
	private:
		bool* callback_bool;
		Settings& m_settings;
	};
}