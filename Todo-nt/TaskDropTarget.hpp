#pragma once
#include <wx/wx.h>
#include <wx/dnd.h>
#include "MainFrame.hpp"

namespace ToDont
{
	class TaskDropTarget : public wxTextDropTarget
	{
	public:
		TaskDropTarget(wxWindow* owner, std::function<void(wxPoint)> dropCallback);
		
		bool OnDropText(wxCoord x, wxCoord y, const wxString& text) override;

	private:
		wxWindow* m_owner;
		std::function<void(wxPoint)> m_dropCallback;
	};
}
