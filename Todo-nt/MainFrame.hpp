#pragma once
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/collpane.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <fstream>
#include <string>
#include <memory>
#include <stdexcept>

#include "TaskButton.hpp"
#include "TaskElement.hpp"
#include "TaskListTitle.hpp"
#include "Settings.hpp"

namespace ToDont
{

	class TaskElement;

	constexpr long FRAME_STYLE = wxDEFAULT_FRAME_STYLE & ~(wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER);

	class MainFrame : public wxFrame
	{
	public:
		MainFrame(wxSize windowSize, std::unique_ptr<Settings> settings);

		void OnMouseDown(wxMouseEvent& event);
		void OnMouseUp(wxMouseEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void SaveEvent(wxCommandEvent& event);
		void LoadEvent(wxCommandEvent& event);

		void HandleDropAtPosition(wxPoint point);
		void ReorderDraggedElement(size_t target);
		
		void SetDraggedTask(TaskElement* task);

		const Settings& GetSettings() const { return *m_settings; }
		const Settings* GetSettingsPtr() const
		{
			if (!m_settings)
				throw std::runtime_error("Settings not valid!");
			return m_settings.get();
		}

	private:
		wxPoint m_dragStart, m_frameStart;
		wxScrolledWindow* m_scroll;
		wxFlexGridSizer* m_grid;
		wxPanel* m_canvas;
		TaskButton* m_addBtn;
		TaskListTitle* m_title;
		wxCollapsiblePane* m_completedPane;
		wxBoxSizer* m_completedSizer;
		wxBoxSizer* m_box;
		std::string m_workingFile;

		std::unique_ptr<Settings> m_settings;
		
		std::optional<TaskElement*> m_draggedTask;

		bool is_settingsOpen = false;

		void Load(std::ifstream& stream);
		void Save(std::string path) const;
		void ClearGridOfElements();

		int IndexOf(wxSizer* s, wxWindow* w) const;
		void WireTask(TaskElement* t);
		void MoveTaskToActive(TaskElement* t);
		void MoveTaskToCompleted(TaskElement* t);

	};
}