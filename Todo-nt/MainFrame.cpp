#include "MainFrame.hpp"
#include "TaskUtils.hpp"
#include "SettingsFrame.hpp"
#include "TaskDropTarget.hpp"
#include "utils.hpp"

using json = nlohmann::json;
namespace ToDont
{
	MainFrame::MainFrame(wxSize windowSize, std::unique_ptr<Settings> settings)
		: wxFrame(nullptr, wxID_ANY, "Todo-nt", wxDefaultPosition, windowSize, FRAME_STYLE),
		m_settings(std::move(settings))
	{


		Bind(wxEVT_LEFT_UP, &MainFrame::OnMouseUp, this);
		Bind(wxEVT_LEFT_DOWN, &MainFrame::OnMouseDown, this);
		Bind(wxEVT_MOTION, &MainFrame::OnMouseMove, this);
		Bind(wxEVT_CHAR_HOOK, &MainFrame::OnKeyDown, this);

		// On Window Close
		Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event)
			{
				this->m_settings->SetLastOpen(m_workingFile);
				if (!this->m_workingFile.empty())
				{
					Save(this->m_workingFile);
				}
				event.Skip();
			});

		// Context Menu
		Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event)
			{
				int id_quit = wxWindow::NewControlId();
				int id_save = wxWindow::NewControlId();
				int id_load = wxWindow::NewControlId();
				int id_settings = wxWindow::NewControlId();
				int id_ontop = wxWindow::NewControlId();
				int id_new = wxWindow::NewControlId();

				wxMenu menu;
				menu.Append(id_new, "New");
				menu.Append(id_save, "Save");
				menu.Append(id_load, "Load");
				menu.AppendSeparator();
				menu.AppendCheckItem(id_ontop, "Always On Top?");
				menu.Check(id_ontop, m_settings->GetOntop());
				menu.AppendSeparator();
				menu.Append(id_settings, "Settings");
				menu.AppendSeparator();
				menu.Append(id_quit, "Quit?");

				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						m_workingFile = "";
						ClearGridOfElements();
						m_title->SetLabel("Title");
						m_scroll->FitInside();
					}, id_new);
				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						Close(true);
					}, id_quit);
				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						if (!is_settingsOpen)
						{
							auto* settingsFrame = new SettingsFrame(this, this->GetSize(), &is_settingsOpen, *this->m_settings);
							settingsFrame->CenterOnParent();
							settingsFrame->Show();
						}
					}, id_settings);
				Bind(wxEVT_MENU, [this](wxCommandEvent& event)
					{
						auto style = this->GetWindowStyleFlag();
						if (event.IsChecked())
							style |= wxSTAY_ON_TOP;
						else
							style &= ~wxSTAY_ON_TOP;
						this->SetWindowStyleFlag(style);
						m_settings->SetOntop(event.IsChecked());
					}, id_ontop);
				Bind(wxEVT_MENU, &MainFrame::SaveEvent, this, id_save);
				Bind(wxEVT_MENU, &MainFrame::LoadEvent, this, id_load);

				PopupMenu(&menu);
			});
			


		SetBackgroundColour(m_settings->GetTheme().bgColor);
		if (m_settings->GetOntop())
			SetWindowStyleFlag(GetWindowStyleFlag() | wxSTAY_ON_TOP);

		m_box = new wxBoxSizer(wxVERTICAL);
		m_grid = new wxFlexGridSizer(0, 1, 2, 10);
		m_scroll = new InvisibleScrollWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
		m_scroll->SetScrollRate(0, 10);
		m_canvas = new wxPanel(m_scroll);
		m_grid->AddGrowableCol(0);
		m_grid->SetMinSize(wxSize(m_scroll->GetClientSize().GetWidth(), -1));
		m_canvas->SetSizer(m_grid);

		m_scroll->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {
			this->GetEventHandler()->ProcessEvent(event);
			});

		m_scroll->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
			this->GetEventHandler()->ProcessEvent(event);
			});

		m_scroll->Bind(wxEVT_MOTION, [this](wxMouseEvent& event) {
			this->GetEventHandler()->ProcessEvent(event);
			});

		m_scroll->SetDropTarget(new TaskDropTarget(m_canvas, [this](wxPoint point)
			{
				HandleDropAtPosition(point);
			}));

		m_addBtn = new TaskButton(m_canvas, wxID_ANY, wxSize(20, 20), "Add");
		m_addBtn->SetTheme(m_settings->GetTheme());
		m_addBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
			if (auto label = ToDont::GetTaskString(m_canvas, ""); label.has_value())
			{
				auto task = new TaskElement(m_canvas, label.value(), *this->m_settings);
				WireTask(task);
				MoveTaskToActive(task);
				m_scroll->FitInside();
			}
		});

		
		m_grid->Add(m_addBtn, 0, wxGROW | wxALL, 10);

		m_completedPane = new wxCollapsiblePane(m_canvas, wxID_ANY, "Completed",
			wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE);
		m_grid->Add(m_completedPane, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

		// Completed pane setup
		{
			auto* pane = m_completedPane->GetPane();
			for (auto* child : m_completedPane->GetChildren())
			{
				if (child != pane)
				{
					child->SetForegroundColour(m_settings->GetTheme().fgColor);
					child->Refresh();
				}
			}

			auto* paneBox = new wxBoxSizer(wxVERTICAL);
			m_completedSizer = new wxBoxSizer(wxVERTICAL);
			paneBox->AddSpacer(4);
			paneBox->Add(m_completedSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 4);
			pane->SetSizer(paneBox);
			m_completedPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&)
			{
				CallAfter([this] {
					if (m_completedPane) m_completedPane->GetPane()->Layout();
					m_canvas->Layout();
					m_scroll->Layout();
					m_scroll->FitInside();
					Layout();
					Refresh(false);
				});
			});
		}


		auto* scrollBox = new wxBoxSizer(wxVERTICAL);
		scrollBox->Add(m_canvas, 0, wxEXPAND);
		m_scroll->SetSizer(scrollBox);
		m_canvas->Layout();
		m_scroll->FitInside();

		m_title = new TaskListTitle(this, "Title", m_settings->GetTheme(), wxSize(25, 25));
		auto* line = new wxStaticLine(this);

		m_box->Add(m_title, 0, wxEXPAND | wxALL, 10);
		m_box->Add(line, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
		m_box->Add(m_scroll, 1, wxEXPAND);


		m_settings->SetUpdatedCallback(std::bind(&MainFrame::SettingsUpdated, this));

		auto lastOpen = m_settings->GetLastOpen();
		if (m_settings->GetShouldOpenLast() && lastOpen.has_value())
		{
			std::ifstream load_file(lastOpen.value());
			if (!load_file)
			{
				wxLogError("Could not open selected file!");
				return;
			}
			m_workingFile = lastOpen.value();
			Load(load_file);
		}
			

		SetSizer(m_box);
		Layout();

	}

	void MainFrame::Save(std::string path) const
	{
		json save;
		save["tasks"] = json::array();
		save["title"] = m_title->GetLabel().ToStdString();
		for (const auto* task : m_grid->GetChildren())
		{
			const auto* casted = wxDynamicCast(task->GetWindow(), TaskElement);
			if (casted)
			{
				json obj =
					json::object({
						{"task", casted->GetButton()->GetCurrentLabel().ToStdString()},
						{"completed", casted->GetCheckBox()->IsChecked()},
						{"important", casted->GetImportant()},
						});
				save["tasks"].push_back(obj);
			}
		}

		if (m_completedSizer)
		{
			for (const auto* item : m_completedSizer->GetChildren())
			{
				const auto* casted = wxDynamicCast(item->GetWindow(), TaskElement);
				if (!casted)continue;
				json obj =
					json::object({
						{"task", casted->GetButton()->GetCurrentLabel().ToStdString()},
						{"completed", casted->GetCheckBox()->IsChecked()},
						{"important", casted->GetImportant()},
						});
				save["tasks"].push_back(obj);
			}
		}

		std::ofstream save_file(path);
		if (!save_file)
		{
			wxLogError("Could not open file to save!");
			return;
		}
		save_file << save << std::endl;
		save_file.close();
		
	}

	void MainFrame::SaveEvent(wxCommandEvent& event)
	{

		
		wxFileDialog saveDialog(
			this,
			"Save Task List",
			"",
			m_workingFile,
			"JSON files (*.json)|*.json",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT
		);

		if (saveDialog.ShowModal() == wxID_OK)
		{
			m_workingFile = saveDialog.GetPath().ToStdString();
			Save(m_workingFile);
		}
		else
			wxLogError("Failed to select file save location!");
	}

	void MainFrame::Load(std::ifstream& stream)
	{
		auto data = json::parse(stream);
		stream.close();
		if (data.contains("title") && data["title"].is_string())
		{
			std::string t = data["title"];
			m_title->SetLabel(t);
		}
		if (data["tasks"].is_array())
		{
			ClearGridOfElements();
			for (const auto& task : data["tasks"])
			{
				if (!task.is_object())continue;
				std::string task_str;
				bool completed = false, important = false;

				if (task.contains("task") && task["task"].is_string())
					task_str = task["task"];
				else
					continue;

				if (task.contains("completed") && task["completed"].is_boolean())
					completed = task["completed"];

				if (task.contains("important") && task["important"].is_boolean())
					important = task["important"];

				auto new_task = new TaskElement(m_scroll, task_str, *m_settings);
				new_task->GetCheckBox()->SetValue(completed);
				new_task->SetImportant(important);
				WireTask(new_task);
				if (completed) MoveTaskToCompleted(new_task);
				else MoveTaskToActive(new_task);
			}
			m_scroll->FitInside();
		}
		else
			wxLogError("No tasks in file!"); return;
	}

	void MainFrame::LoadEvent(wxCommandEvent& event)
	{

		// TODO: ask to save first

		wxMessageDialog message(this, "Save?", "Save Before Loading?", wxYES_NO | wxCANCEL);

		switch (message.ShowModal())
		{
		case wxID_YES:
			this->SaveEvent(event);
			break;
		case wxID_NO:
			break;
		case wxID_CANCEL:
		default:
			return;
			break;
		}

		wxFileDialog loadDialog(
			this,
			"Open Task List",
			"",
			"tasks.json",
			"JSON files (*.json)|*.json",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST
		);

		if (loadDialog.ShowModal() != wxID_OK)
		{
			wxLogError("Failed to select file to load!");
			return;
		}
		auto path = loadDialog.GetPath().ToStdString();
		std::ifstream load_file(path);
		if (!load_file)
		{
			wxLogError("Could not open selected file!");
			return;
		}

		m_workingFile = path;
		Load(load_file);
		
	}

	void MainFrame::ClearGridOfElements()
	{
		{
			std::vector<TaskElement*> tbd;
			for (const auto* task : m_grid->GetChildren())
			{
				auto* casted = wxDynamicCast(task->GetWindow(), TaskElement);
				if (casted)
					tbd.push_back(casted);
			}
			for (auto& task : tbd)
			{
				m_grid->Detach(task);
				task->Destroy();
			}
		}
		{
			std::vector<TaskElement*> tbd;
			for (const auto* task : m_completedSizer->GetChildren())
			{
				auto* casted = wxDynamicCast(task->GetWindow(), TaskElement);
				if (casted)
					tbd.push_back(casted);
			}
			for (auto& task : tbd)
			{
				m_completedSizer->Detach(task);
				task->Destroy();
			}
		}
		m_scroll->Layout();
	}

	void MainFrame::SetDraggedTask(TaskElement* task)
	{
		m_draggedTask = task;
	}

	void MainFrame::ReorderDraggedElement(size_t target)
	{
		if (!m_draggedTask.has_value() || !m_draggedTask.value()) return;
			//crash
		m_grid->Detach(m_draggedTask.value());
		m_grid->Insert(target, m_draggedTask.value(), 1, wxEXPAND | wxALL, 5);
		m_scroll->Layout();
		m_scroll->FitInside();
	}

	void MainFrame::HandleDropAtPosition(wxPoint point)
	{
		for (size_t i = 0; i < m_grid->GetItemCount(); i++)
		{
			auto item = m_grid->GetItem(i)->GetWindow();
			if (!item) continue;
			if (item->GetRect().Contains(point) && dynamic_cast<TaskElement*>(item))
			{
				//Reorder
				ReorderDraggedElement(i);
				break;
			}
		}
	}

	void MainFrame::OnMouseMove(wxMouseEvent& event)
	{
		if (event.Dragging() && event.LeftIsDown())
		{
			wxPoint pos = ClientToScreen(event.GetPosition());
			Move(m_frameStart + (pos - m_dragStart));
		}
	}

	void MainFrame::OnKeyDown(wxKeyEvent& event)
	{
		int key = event.GetKeyCode();
		switch (key)
		{
		case WXK_ESCAPE:
			Close(true);
			break;
		default:
			event.Skip();
		}
	}

	void MainFrame::OnMouseUp(wxMouseEvent& event)
	{
		if (HasCapture()) ReleaseMouse();
	}

	void MainFrame::OnMouseDown(wxMouseEvent& event)
	{
		CaptureMouse();
		m_dragStart = ClientToScreen(event.GetPosition());
		m_frameStart = GetPosition();
	}

	int MainFrame::IndexOf(wxSizer* s, wxWindow* w) const
	{
		if (!s || !w)return -1;
		for (size_t i = 0; i < s->GetItemCount(); i++)
			if (auto* item = s->GetItem(i); item && item->GetWindow() == w) 
				return static_cast<int>(i);
		return -1;
	}

	void MainFrame::WireTask(TaskElement* t)
	{
		if (!t) return;
		t->GetCheckBox()->Bind(wxEVT_CHECKBOX, [this, t](wxCommandEvent& e)
			{
				if (t->GetCheckBox()->IsChecked())
				{
					t->PlayCompletedSound();
					MoveTaskToCompleted(t);
				}
				else
					MoveTaskToActive(t);

				m_scroll->FitInside();
				m_scroll->Layout();
				Layout();
			});
	}

	void MainFrame::MoveTaskToActive(TaskElement* t)
	{
		if (!t) return;

		m_grid->Detach(t);
		if (m_completedSizer) m_completedSizer->Detach(t);

		t->Reparent(m_canvas);
		t->Show();

		int completedIdx = IndexOf(m_grid, m_completedPane);
		if (completedIdx < 0) completedIdx = static_cast<int>(m_grid->GetItemCount());
		m_grid->Insert(completedIdx, t, 0, wxEXPAND | wxALL, 5);
		m_completedPane->Layout();

		//m_grid->FitInside(m_scroll);
		m_canvas->Layout();
		m_scroll->FitInside();
		FitInside();

	}

	void MainFrame::MoveTaskToCompleted(TaskElement* t)
	{
		if (!t || !m_completedSizer) return;
		m_grid->Detach(t);

		t->Reparent(m_completedPane->GetPane());
		t->Show();

		m_completedSizer->Prepend(t, 0, wxEXPAND | wxALL, 5);
		if (!m_completedPane->IsExpanded())
			m_completedPane->Expand();

		auto* pane = m_completedPane->GetPane();
		pane->InvalidateBestSize();
		m_completedPane->InvalidateBestSize();
		pane->Layout();
		m_completedPane->Layout();

		//m_grid->FitInside(m_scroll);
		m_canvas->Layout();
		m_scroll->FitInside();


		Layout();
	}

	void MainFrame::SettingsUpdated()
	{
		auto theme = this->m_settings->GetTheme();
		UpdateChildrenThemes(m_grid, theme);
		UpdateChildrenThemes(m_completedSizer, theme);
		UpdateChildrenThemes(m_box, theme);

		for (auto* child : m_completedPane->GetChildren())
		{
			if (child != m_completedPane->GetPane())
			{
				child->SetForegroundColour(theme.fgColor);
				child->Refresh();
			}
		}
		this->m_addBtn->SetTheme(theme);
		this->m_addBtn->Refresh();
		this->SetBackgroundColour(theme.bgColor);
		this->SetForegroundColour(theme.fgColor);
		this->Refresh();
		Layout();
	}

}
