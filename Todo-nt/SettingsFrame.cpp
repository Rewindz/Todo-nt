#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

#include "SettingsFrame.hpp"
#include "CustomFrame.hpp"
#include "TaskUtils.hpp"
#include "TaskButton.hpp"

using json = nlohmann::json;

namespace ToDont
{
	SettingsFrame::SettingsFrame(wxWindow* parent, wxSize windowSize, bool* settings_open, Settings& settings)
		: wxFrame(parent, wxID_ANY, "Settings", wxDefaultPosition, windowSize, SETTINGS_FRAME_STYLE),
		callback_bool(settings_open), m_settings(settings)
	{
		SetIcons(GetAppIcons());
		const auto& s_theme = m_settings.GetTheme();
		SetBackgroundColour(s_theme.bgColor);
		SetForegroundColour(s_theme.fgColor);

		*callback_bool = true;
		Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event)
			{
				*this->callback_bool = false;
				event.Skip();
			});

		Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event)
			{

			});

		auto* sizer = new wxBoxSizer(wxVERTICAL);

		{
			wxArrayString a;
			for (const auto& theme : GetAvailableThemes())
				a.push_back(theme.name);
			auto* themeLbl = new wxStaticText(this, wxID_ANY, "Theme");
			SetFontSize(themeLbl, 14);
			themeLbl->SetWindowStyle(wxALIGN_CENTER_HORIZONTAL);
			auto* theme = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, a);
			int i = theme->FindString(m_settings.GetTheme().name, true);
			if (i != wxNOT_FOUND)
				theme->SetSelection(i);

			theme->Bind(wxEVT_CHOICE, [this, theme](wxCommandEvent& event)
				{
					wxString newVal = theme->GetStringSelection();
					this->m_settings.SetTheme(TaskTheme::GetThemeByName(newVal.ToStdString()).value_or(this->m_settings.GetTheme()));
					auto theme = this->m_settings.GetTheme();
					this->SetBackgroundColour(theme.bgColor);
					this->SetForegroundColour(theme.fgColor);
					for (auto item : this->GetSizer()->GetChildren())
					{
						auto window = item->GetWindow();
						if (!window)continue;
						auto btn = dynamic_cast<TaskButton*>(window);
						if (btn)
							btn->SetTheme(theme);
						else 
						{
							window->SetBackgroundColour(theme.bgColor);
							window->SetForegroundColour(theme.fgColor);
						}
					}
					this->Refresh();
				});

			sizer->Add(themeLbl, 0, wxEXPAND | wxALL, 5);
			sizer->Add(theme, 0, wxEXPAND | wxALL, 10);
		}

		{
			wxArrayString a;
			for (const auto& sound : GetAvailableSounds())
				a.push_back(sound.name);
			auto* cSoundLbl = new wxStaticText(this, wxID_ANY, "Completed Sound");
			SetFontSize(cSoundLbl, 14);
			cSoundLbl->SetWindowStyle(wxALIGN_CENTER_HORIZONTAL);
			auto* cSound = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, a);
			int i = cSound->FindString(m_settings.GetCompleteSound().name, true);
			if (i != wxNOT_FOUND)
				cSound->SetSelection(i);

			cSound->Bind(wxEVT_CHOICE, [this, cSound](wxCommandEvent& event)
				{
					wxString newVal = cSound->GetStringSelection();
					this->m_settings.SetCompleteSound(TaskSound::GetSoundByName(newVal.ToStdString()).value_or(this->m_settings.GetCompleteSound()));
				});

			sizer->Add(cSoundLbl, 0, wxEXPAND | wxALL, 5);
			sizer->Add(cSound, 0, wxEXPAND | wxALL, 10);
		}

		{
			wxArrayString a;
			for (const auto& sound : GetAvailableSounds())
				a.push_back(sound.name);
			auto* cSoundLbl = new wxStaticText(this, wxID_ANY, "Deleted Sound");
			SetFontSize(cSoundLbl, 14);
			cSoundLbl->SetWindowStyle(wxALIGN_CENTER_HORIZONTAL);
			auto* cSound = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, a);
			int i = cSound->FindString(m_settings.GetDeleteSound().name, true);
			if (i != wxNOT_FOUND)
				cSound->SetSelection(i);

			cSound->Bind(wxEVT_CHOICE, [this, cSound](wxCommandEvent& event)
				{
					wxString newVal = cSound->GetStringSelection();
					this->m_settings.SetDeleteSound(TaskSound::GetSoundByName(newVal.ToStdString()).value_or(this->m_settings.GetDeleteSound()));
				});

			sizer->Add(cSoundLbl, 0, wxEXPAND | wxALL, 5);
			sizer->Add(cSound, 0, wxEXPAND | wxALL, 10);
		}

		{
			auto* shouldOpenChk = new wxCheckBox(this, wxID_ANY, "Open Last File On Start?");
			shouldOpenChk->SetValue(m_settings.GetShouldOpenLast());
			shouldOpenChk->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent& event)
				{
					this->m_settings.SetShouldOpenLast(event.IsChecked());
				});
			sizer->Add(shouldOpenChk, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP| wxBOTTOM, 5);
		}

		{
			auto* customSoundBtn = new TaskButton(this, wxID_ANY, wxSize(-1, 35), "Add Custom Sound");
			customSoundBtn->SetTheme(m_settings.GetTheme());
			customSoundBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
				{
					wxFileDialog loadDialog(
						this,
						"Open Sound File",
						"",
						"",
						"WAV files (*.wav)|*.wav",
						wxFD_OPEN | wxFD_FILE_MUST_EXIST
					);

					if (loadDialog.ShowModal() != wxID_OK)
					{
						wxLogError("Failed to select sound file to load!");
						return;
					}

					TaskSound s;
					s.custom = true;
					s.name = loadDialog.GetFilename();
					s.filePath = loadDialog.GetPath();
					m_settings.SubmitSound(s);

				});
			sizer->Add(customSoundBtn, 0, wxEXPAND | wxALL, 5);
		}

		{
			auto* openCustom = new TaskButton(this, wxID_ANY, wxSize(-1, 35), "Create Custom Theme");
			openCustom->SetTheme(m_settings.GetTheme());
			openCustom->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) 
				{
					auto* custom = new CustomFrame(this, this->GetSize() * 2, m_settings);
					custom->CenterOnScreen();
					custom->Show();
				});
			sizer->Add(openCustom, 0, wxEXPAND | wxALL, 5);
		}
		
		SetSizer(sizer);
		Layout();

	}
}