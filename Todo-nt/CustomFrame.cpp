#include "CustomFrame.hpp"
#include "TaskButton.hpp"
#include <wx/scrolwin.h>
#include <wx/clrpicker.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/dialog.h>

#include <ranges>

namespace ToDont
{
	CustomFrame::CustomFrame(wxWindow* parent, wxSize windowSize, Settings& settings)
		: wxFrame(parent, wxID_ANY, "Custom Settings", wxDefaultPosition, windowSize, CUSTOM_FRAME_STYLE),
		m_settings(settings)
	{
		SetIcons(GetAppIcons());
		auto currentTheme = m_settings.GetTheme();
		m_createdTheme = currentTheme;
		m_createdTheme.name = "Custom Theme";
		m_createdTheme.custom = true;
		SetBackgroundColour(currentTheme.bgColor);
		SetForegroundColour(currentTheme.fgColor);
		m_sizer = new wxBoxSizer(wxVERTICAL);
		auto* scrollSizer = new wxBoxSizer(wxVERTICAL);
		auto* scrollBox = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
		scrollBox->SetScrollRate(0, 10);
		scrollBox->SetForegroundColour(currentTheme.fgColor);
		{
			auto* lbl = new wxStaticText(scrollBox, wxID_ANY, "Theme Name");
			SetFontSize(lbl, 14);
			auto* nameInput = new wxTextCtrl(scrollBox, wxID_ANY, m_createdTheme.name);

			nameInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event)
				{
					m_createdTheme.name = event.GetString();
				});

			scrollSizer->Add(lbl, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 25);
			scrollSizer->Add(nameInput, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
		}
		{
			auto* lbl = new wxStaticText(scrollBox, wxID_ANY, "Background Colour");
			SetFontSize(lbl, 14);
			auto* picker = new wxColourPickerCtrl
			(scrollBox, wxID_ANY, currentTheme.bgColor, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);

			picker->Bind(wxEVT_COLOURPICKER_CHANGED, [this, scrollSizer](wxColourPickerEvent& event)
				{
					m_createdTheme.bgColor = event.GetColour();
					UpdateSelfTheme(scrollSizer);
				});

			scrollSizer->Add(lbl, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 25);
			scrollSizer->Add(picker, wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT | wxRIGHT, 50).Border(wxTOP | wxBOTTOM, 10));
		}
		{
			auto* lbl = new wxStaticText(scrollBox, wxID_ANY, "Foreground Colour");
			SetFontSize(lbl, 14);
			auto* picker = new wxColourPickerCtrl
			(scrollBox, wxID_ANY, currentTheme.fgColor, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);

			picker->Bind(wxEVT_COLOURPICKER_CHANGED, [this, scrollSizer](wxColourPickerEvent& event)
				{
					m_createdTheme.fgColor = event.GetColour();
					UpdateSelfTheme(scrollSizer);
				});

			scrollSizer->Add(lbl, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 25);
			scrollSizer->Add(picker, wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT | wxRIGHT, 50).Border(wxTOP | wxBOTTOM, 10));
		}
		{
			auto* lbl = new wxStaticText(scrollBox, wxID_ANY, "Hovered Colour");
			SetFontSize(lbl, 14);
			auto* picker = new wxColourPickerCtrl
			(scrollBox, wxID_ANY, currentTheme.hover, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);

			picker->Bind(wxEVT_COLOURPICKER_CHANGED, [this, scrollSizer](wxColourPickerEvent& event)
				{
					m_createdTheme.hover = event.GetColour();
					UpdateSelfTheme(scrollSizer);
				});

			scrollSizer->Add(lbl, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 25);
			scrollSizer->Add(picker, wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT | wxRIGHT, 50).Border(wxTOP | wxBOTTOM, 10));
		}
		{
			auto* lbl = new wxStaticText(scrollBox, wxID_ANY, "Clicked Colour");
			SetFontSize(lbl, 14);
			auto* picker = new wxColourPickerCtrl
			(scrollBox, wxID_ANY, currentTheme.active, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);

			picker->Bind(wxEVT_COLOURPICKER_CHANGED, [this, scrollSizer](wxColourPickerEvent& event)
				{
					m_createdTheme.active = event.GetColour();
					UpdateSelfTheme(scrollSizer);
				});

			scrollSizer->Add(lbl, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 25);
			scrollSizer->Add(picker, wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT | wxRIGHT, 50).Border(wxTOP | wxBOTTOM, 10));
		}
		{
			auto* createBtn = new TaskButton(scrollBox, wxID_ANY, wxSize(-1, 25), "Create Theme!");
			createBtn->SetTheme(currentTheme);

			createBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
				{
					auto exists = std::ranges::find(GetAvailableThemes(), m_createdTheme.name, &TaskTheme::name);
					if (exists != GetAvailableThemes().end())
					{
						if (exists->custom)
						{
							wxMessageDialog msg(this, 
								"A theme with the same name already exists. Do you want to replace it?",
								"Replace Theme?", wxYES_NO);
							if (msg.ShowModal() != wxID_YES)
								return;
						}
						else
						{
							wxLogMessage("Can not replace a built in theme!");
							return;
						}
					}

					this->m_settings.SubmitTheme(this->m_createdTheme);
					this->Close(true);
				});

			scrollSizer->Add(createBtn, 0, wxEXPAND | wxALL, 25);
		}


		scrollBox->SetSizer(scrollSizer);
		scrollBox->FitInside();
		scrollBox->Layout();
		m_sizer->Add(scrollBox, 1, wxEXPAND | wxALL, 5);

	}

	CustomFrame::~CustomFrame()
	{

	}

	void CustomFrame::UpdateSelfTheme(const wxSizer* sizer)
	{
		SetBackgroundColour(m_createdTheme.bgColor);
		SetForegroundColour(m_createdTheme.fgColor);
		for (auto child : sizer->GetChildren())
		{
			auto window = child->GetWindow();
			if (!window) continue;
			auto btn = dynamic_cast<TaskButton*>(window);
			if (btn)
				btn->SetTheme(m_createdTheme);
			else
			{
				window->SetForegroundColour(m_createdTheme.fgColor);
			}
		}
		Refresh();
	}
}