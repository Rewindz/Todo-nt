#include <iostream>
#include <wx/app.h>

#include "TaskDropTarget.hpp"
#include "MainFrame.hpp"
#include "Settings.hpp"
#include "TaskUtils.hpp"

class Application : public wxApp
{
public:
	virtual bool OnInit() override;
};



wxIMPLEMENT_APP(Application);

bool Application::OnInit()
{
	auto settings = std::make_unique<ToDont::Settings>();
	ToDont::MainFrame* frame = new ToDont::MainFrame(wxSize(300, 400), std::move(settings));
	if (!frame) return false;
	frame->SetIcons(ToDont::GetAppIcons());
	frame->CenterOnScreen();
	frame->Show(true);

	return true;
}