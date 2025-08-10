#pragma once
#include <wx/window.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/timer.h>
#include <wx/sound.h>
#include <wx/dnd.h>
#include "TaskButton.hpp"
#include "Settings.hpp"

namespace ToDont
{
	class TaskElement : public wxPanel
	{
	public:
		TaskElement(wxWindow* parent, const wxString& label, const Settings& settings);

		wxCheckBox* GetCheckBox() const;
		TaskButton* GetButton() const;
		
		void AnimateDestroy(std::function<void()> onDone);
		void UpdateTheme();
		void PlayCompletedSound();

	private:
		wxCheckBox* m_CheckBox;
		TaskButton* m_taskButton;
		std::unique_ptr<wxSound> m_completedSound;
		std::unique_ptr<wxSound> m_deletedSound;
		bool is_completed;
		std::function<void()> m_onDestroyCallback;
		wxTimer m_animTimer;
		int m_currentHeight;
		const Settings& m_settings;

		void OnPaint(wxPaintEvent& event);
		void OnAnimTick(wxTimerEvent& event);
	};
}