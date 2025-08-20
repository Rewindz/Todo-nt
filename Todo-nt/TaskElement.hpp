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
#include "TaskCheck.hpp"


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

		//bool GetImportant() const { return m_important->IsChecked(); }
		//void SetImportant(bool b) { m_important->SetValue(b); }

	private:
		wxCheckBox* m_checkBox;
		TaskCheck* m_important;
		TaskButton* m_taskButton;
		std::unique_ptr<wxSound> m_completedSound;
		std::unique_ptr<wxSound> m_deletedSound;
		bool is_completed;
		std::function<void()> m_onDestroyCallback;
		wxTimer m_animTimer;
		int m_currentHeight;
		const Settings& m_settings;

		void OnAnimTick(wxTimerEvent& event);
	};
}