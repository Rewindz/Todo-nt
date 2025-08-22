#include "TaskElement.hpp"
#include "MainFrame.hpp"
#include "TaskUtils.hpp"

namespace ToDont
{

	TaskElement::TaskElement(wxWindow* parent, const wxString& label, const Settings& settings)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), 
		is_completed(false), m_settings(settings)
	{
		m_completedSound = std::make_unique<wxSound>(settings.GetCompleteSound().filePath);
		m_deletedSound = std::make_unique<wxSound>(settings.GetDeleteSound().filePath);
		m_animTimer.SetOwner(this);
		m_currentHeight = GetSize().y;

		Bind(wxEVT_TIMER, &TaskElement::OnAnimTick, this);

		Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event)
			{
				if (event.LeftIsDown())
				{
					auto* parentFrame = wxDynamicCast(wxGetTopLevelParent(this), MainFrame);
					if (parentFrame)
						parentFrame->SetDraggedTask(this);
					else
						return;

					wxTextDataObject data("TaskDrag");
					wxDropSource dragSource(this);
					dragSource.SetData(data);
					dragSource.DoDragDrop(wxDrag_CopyOnly);
				}
			});

		Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event)
			{
				this->SetBackgroundColour(m_settings.GetTheme().hover);
				this->Refresh();
			});

		Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event)
			{
				this->SetBackgroundColour(m_settings.GetTheme().bgColor);
				this->Refresh();
			});

		Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) 
			{
				int id_edit = wxWindow::NewControlId();
				int id_delete = wxWindow::NewControlId();

				wxMenu menu;
				menu.Append(id_edit, "Edit");
				menu.Append(id_delete, "Delete");

				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						AnimateDestroy([this]()
							{
								auto* parent = GetParent();
								parent->GetSizer()->Detach(this);
								parent->Layout();
							});
						
					}, id_delete);

				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						if (auto label = ToDont::GetTaskString(this->GetParent(), this->m_taskButton->GetCurrentLabel()); label.has_value())
						{
							this->m_taskButton->SetLabel(label.value());
							this->Refresh();
						}
					}, id_edit);

				PopupMenu(&menu);
			});

		auto sizer = new wxBoxSizer(wxHORIZONTAL);
		m_checkBox = new wxCheckBox(this, wxID_ANY, "");
		m_taskButton = new TaskButton(this, wxID_ANY, this->GetSize(), label);
		m_taskButton->SetTheme(m_settings.GetTheme());
		m_taskButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) 
		{
				if (auto label = ToDont::GetTaskString(this->GetParent(), this->m_taskButton->GetCurrentLabel()); label.has_value())
				{
					this->m_taskButton->SetLabel(label.value());
					this->Refresh();
				}
		});
		m_important = new TaskCheck(m_settings, this, wxID_ANY);



		sizer->Add(m_checkBox, 0, wxSTRETCH_NOT | wxALIGN_CENTRE_VERTICAL | wxALL, 10);
		sizer->Add(m_taskButton, 1, wxGROW | wxALL, 10);
		sizer->Add(m_important, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
		this->SetBackgroundColour(m_settings.GetTheme().bgColor);
		this->SetForegroundColour(m_settings.GetTheme().fgColor);
		this->SetSizerAndFit(sizer);
	}

	void TaskElement::PlayCompletedSound()
	{
		m_completedSound->Play(wxSOUND_ASYNC);
	}

	void TaskElement::AnimateDestroy(std::function<void()> onDone)
	{
		m_onDestroyCallback = onDone;
		m_animTimer.Start(10);
	}

	void TaskElement::OnAnimTick(wxTimerEvent& event)
	{
		m_currentHeight -= 5;
		if (m_currentHeight <= 0)
		{
			m_animTimer.Stop();
			if (m_onDestroyCallback) m_onDestroyCallback();
			m_deletedSound->Play(wxSOUND_ASYNC);
			Destroy();
			return;
		}
		SetMinSize(wxSize(-1, m_currentHeight));
		GetParent()->Layout();
	}

	void TaskElement::UpdateTheme()
	{
		auto theme = m_settings.GetTheme();
		SetBackgroundColour(theme.bgColor);
		SetForegroundColour(theme.fgColor);
		m_taskButton->SetTheme(theme);
		m_taskButton->Refresh();

		//wxSound::Stop();
		m_completedSound = std::make_unique<wxSound>(m_settings.GetCompleteSound().filePath);
		m_deletedSound = std::make_unique<wxSound>(m_settings.GetDeleteSound().filePath);
		
		Refresh();

	}

}
