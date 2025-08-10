#include "TaskDropTarget.hpp"

namespace ToDont
{
	TaskDropTarget::TaskDropTarget(wxWindow* owner, std::function<void(wxPoint)> dropCallback)
		: m_owner(owner), m_dropCallback(dropCallback)
	{

	}

	bool TaskDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& text)
	{
		//wxPoint local = m_owner->ScreenToClient(wxPoint(x, y));
		m_dropCallback(wxPoint(x, y));
		return true;
	}
}