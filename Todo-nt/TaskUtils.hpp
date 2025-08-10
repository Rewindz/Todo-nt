#pragma once

#include <optional>
#include <ranges>
#include <string>
#include <wx/colour.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/font.h>
#include <wx/stattext.h>
#include <wx/iconbndl.h>
#include <wx/icon.h>

namespace ToDont
{

	struct TaskSound 
	{
		std::string name;
		std::string filePath;
        bool custom = false;
        static bool UpsertByName(std::vector<TaskSound>& vec, const TaskSound& t);
        static std::optional<TaskSound> GetSoundByName(const std::string& name);
	};

	struct TaskTheme
	{
		std::string name;
		wxColour bgColor;
		wxColour fgColor;
        wxColour hover;
        wxColour active;
        bool custom = false;
        static bool UpsertByName(std::vector<TaskTheme>& vec, const TaskTheme& t);
        static std::optional<TaskTheme> GetThemeByName(const std::string& name);   
	};

	inline const std::vector<TaskSound>& GetAvailableSounds()
	{
		static std::vector<TaskSound> sounds = {
            {"WindowsDing",          "C:\\Windows\\Media\\ding.wav"},
            {"WindowsNotify",        "C:\\Windows\\Media\\Windows Notify.wav"},
            {"WindowsBackground",    "C:\\Windows\\Media\\Windows Background.wav"},
            {"WindowsBalloon",       "C:\\Windows\\Media\\Windows Balloon.wav"},
            {"WindowsCriticalStop",  "C:\\Windows\\Media\\Windows Critical Stop.wav"},
            {"WindowsDefault",       "C:\\Windows\\Media\\Windows Default.wav"},
            {"WindowsExclamation",   "C:\\Windows\\Media\\Windows Exclamation.wav"},
            {"WindowsExit",          "C:\\Windows\\Media\\Windows Exit.wav"},
            {"WindowsFail",          "C:\\Windows\\Media\\Windows Fail.wav"},
            {"WindowsInformation",   "C:\\Windows\\Media\\Windows Information Bar.wav"},
            {"WindowsNotifyCalendar","C:\\Windows\\Media\\Windows Notify Calendar.wav"},
            {"WindowsNotifyEmail",   "C:\\Windows\\Media\\Windows Notify Email.wav"},
            {"WindowsNotifyMessaging","C:\\Windows\\Media\\Windows Notify Messaging.wav"},
            {"WindowsPrintComplete", "C:\\Windows\\Media\\Windows Print complete.wav"},
            {"WindowsRingIn",        "C:\\Windows\\Media\\Windows Ringin.wav"},
            {"WindowsRingOut",       "C:\\Windows\\Media\\Windows Ringout.wav"},
            {"WindowsStartup",       "C:\\Windows\\Media\\Windows Startup.wav"}
		};
		return sounds;
	}

    inline const std::vector<TaskTheme>& GetAvailableThemes()
    {
        static std::vector<TaskTheme> themes = {
            {
                "Dark",
                wxColour(30, 30, 30),
                wxColour(255, 255, 255),
                wxColour(50, 50, 50),
                wxColour(70, 70, 70)
            },
            {
                "ClassicLight",
                wxColour(240, 240, 240),
                wxColour(0, 0, 0),
                wxColour(225, 225, 225),
                wxColour(200, 200, 200)
            },
            // --- Pastel themes ---
            {
                "PastelMint",
                wxColour(202, 241, 234),   // background
                wxColour(40, 60, 55),      // foreground
                wxColour(186, 230, 223),   // hover
                wxColour(162, 217, 208)    // active
            },
            {
                "PastelPeach",
                wxColour(255, 223, 211),
                wxColour(60, 40, 35),
                wxColour(255, 210, 195),
                wxColour(255, 190, 170)
            },
            {
                "PastelLavender",
                wxColour(230, 224, 255),
                wxColour(50, 45, 75),
                wxColour(215, 208, 250),
                wxColour(200, 190, 240)
            },
            {
                "PastelSky",
                wxColour(210, 235, 255),
                wxColour(35, 50, 65),
                wxColour(195, 225, 250),
                wxColour(175, 210, 240)
            },
            {
                "PastelRose",
                wxColour(255, 228, 235),
                wxColour(60, 40, 50),
                wxColour(250, 215, 225),
                wxColour(240, 195, 210)
            },
            // --- A few accent themes for contrast ---
            {
                "OceanBlue",
                wxColour(15, 32, 39),
                wxColour(200, 230, 255),
                wxColour(32, 58, 67),
                wxColour(44, 83, 100)
            },
            {
                "ForestGreen",
                wxColour(27, 38, 29),
                wxColour(200, 255, 200),
                wxColour(38, 58, 38),
                wxColour(56, 94, 56)
            },
            {
                "RetroTerminal",
                wxColour(0, 20, 0),
                wxColour(0, 255, 0),
                wxColour(0, 40, 0),
                wxColour(0, 60, 0)
            }
        };
        return themes;
    }

    inline const wxIconBundle& GetAppIcons()
    {
        static wxIconBundle bundle(wxICON(IDI_APP_ICON));
        return bundle;
    }

	void SetFontSize(wxStaticText* text, double pt);
	
	std::optional<wxString> GetTaskString(wxWindow* parent, wxString existing);
	std::optional<wxString> GetTitleString(wxWindow* parent, wxString existing);
}