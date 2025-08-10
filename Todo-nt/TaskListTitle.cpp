#include "TaskListTitle.hpp"
#include <wx/menu.h>

namespace ToDont
{
	TaskListTitle::TaskListTitle(wxWindow* parent, wxString label, const TaskTheme& theme, wxSize size)
		: wxStaticText(parent, wxID_ANY, label, wxDefaultPosition, size, wxALIGN_CENTER_HORIZONTAL)
	{
		this->SetForegroundColour(theme.fgColor);
		SetFontSize(this, 16);

		Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event)
			{
				int id_edit = wxWindow::NewControlId();
				int id_quit = wxWindow::NewControlId();
				wxMenu menu;
				menu.Append(id_edit, "Edit Title");
				menu.AppendSeparator();
				menu.Append(id_quit, "Quit?");

				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						auto val = GetTitleString(this, this->GetLabel());
						if (val.has_value())
						{
							this->SetLabel(val.value());
							this->InvalidateBestSize();
							this->GetParent()->Layout();
						}
					}, id_edit);

				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						wxGetTopLevelParent(this)->Close(true);
					}, id_quit);
				PopupMenu(&menu);
			});
	}
}