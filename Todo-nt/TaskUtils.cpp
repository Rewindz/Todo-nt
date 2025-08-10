#include "TaskUtils.hpp"

#include <wx/textdlg.h>


namespace ToDont
{

	bool ToDont::TaskTheme::UpsertByName(std::vector<TaskTheme>& vec, const TaskTheme& t)
	{
		if (auto it = std::ranges::find(vec, t.name, &TaskTheme::name); it != vec.end())
		{
			*it = t;
			return true;
		}
		else
			vec.push_back(t);
		return false;
	}

	std::optional<TaskTheme> ToDont::TaskTheme::GetThemeByName(const std::string& name)
	{
		auto it = std::ranges::find_if(GetAvailableThemes(),
			[&name](const TaskTheme& theme) {
				return theme.name == name;
			});
		if (it != GetAvailableThemes().end())
			return *it;
		else
			return std::nullopt;
	}

	std::optional<TaskSound> ToDont::TaskSound::GetSoundByName(const std::string& name)
	{
		auto it = std::ranges::find_if(GetAvailableSounds(),
			[&name](const TaskSound& sound) {
				return sound.name == name;
			});
		if (it != GetAvailableSounds().end())
			return *it;
		else
			return std::nullopt;
	}

	void ToDont::SetFontSize(wxStaticText* text, double pt)
	{
		auto font = text->GetFont();
		font.SetFractionalPointSize(pt);
		text->SetFont(font);
	}

	std::optional<wxString> ToDont::GetTaskString(wxWindow* parent, wxString existing)
	{
		wxTextEntryDialog dlg(parent, "What is the task?", "Task Input", wxEmptyString, wxCENTRE | wxOK | wxCANCEL);
		wxString task_name(".....");
		auto ret = dlg.ShowModal();
		switch (ret)
		{
		case wxID_OK:
			return dlg.GetValue().IsEmpty() ? ((existing.IsEmpty()) ? task_name : existing) : dlg.GetValue();
		default:
		case wxID_CANCEL:
			return std::nullopt;
		}
		return std::nullopt;
	}

	std::optional<wxString> ToDont::GetTitleString(wxWindow* parent, wxString existing)
	{
		wxTextEntryDialog dlg(parent, "What is the title?", "Title", wxEmptyString, wxCENTRE | wxOK | wxCANCEL);
		wxString task_name("Title");
		auto ret = dlg.ShowModal();
		switch (ret)
		{
		case wxID_OK:
			return dlg.GetValue().IsEmpty() ? ((existing.IsEmpty()) ? task_name : existing) : dlg.GetValue();
		default:
		case wxID_CANCEL:
			return std::nullopt;
		}
		return std::nullopt;
	}
}