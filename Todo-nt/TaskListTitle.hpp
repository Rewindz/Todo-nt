#pragma once
#include <wx/statline.h>
#include <wx/window.h>
#include <wx/string.h>
#include "TaskUtils.hpp"

namespace ToDont
{
	class TaskListTitle : public wxStaticText
	{
	public:
		TaskListTitle(wxWindow* parent, wxString label, const TaskTheme& theme, wxSize size = wxDefaultSize);
	private:
	};
}