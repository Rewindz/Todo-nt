#include "Settings.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace ToDont
{
	Settings::Settings(std::string filePath)
		: m_filePath(filePath)
	{
		load();
	}

	Settings::~Settings()
	{
		save();
	}

	void Settings::createDefaultSettings()
	{
		m_selectedCompleteSound = GetAvailableSounds().at(0);
		m_selectedDeleteSound = GetAvailableSounds().at(0);
		m_selectedTheme = GetAvailableThemes().at(0);
		m_shouldOpenLast = true;
		m_ontop = false;
		m_windSize = wxSize(300, 450);
		save();
	}

	std::optional<std::string> Settings::GetLastOpen() const
	{
		if (m_lastOpen.empty())
			return std::nullopt;
		else
			return m_lastOpen;
	}

	void Settings::SubmitTheme(TaskTheme theme)
	{
		auto& themes = const_cast<std::vector<TaskTheme>&>(GetAvailableThemes());
		TaskTheme::UpsertByName(m_customThemes, theme);
		if (TaskTheme::UpsertByName(themes, theme))
		{
			if(m_updatedCb) m_updatedCb();
		}
	}

	void Settings::SubmitSound(TaskSound sound)
	{
		auto& sounds = const_cast<std::vector<TaskSound>&>(GetAvailableSounds());
		TaskSound::UpsertByName(m_customSounds, sound);
		if (TaskSound::UpsertByName(sounds, sound))
		{
			if (m_updatedCb) m_updatedCb();
		}
	}

	bool Settings::load()
	{
		if (m_filePath.empty())
			m_filePath = "./settings.json";

		if (!std::filesystem::exists(m_filePath))
			createDefaultSettings();

		std::ifstream file(m_filePath);
		if (!file)
			return false;

		json json_;
		try {
			json_ = json::parse(file);
		}
		catch (const json::parse_error&)
		{
			file.close();
			return false;
		}
		file.close();

		//set settings from file

		auto custom = json_["custom"];
		if (custom.is_object())
		{
			auto themes = custom["themes"];
			if (themes.is_array())
			{
				for (const auto& theme : themes)
				{
					if (!theme.is_object())continue;
					TaskTheme t;
					t.custom = true;
					t.name = theme["name"];
					t.bgColor = colourFromObject(theme["bg"]);
					t.fgColor = colourFromObject(theme["fg"]);
					t.active = colourFromObject(theme["active"]);
					t.hover = colourFromObject(theme["hover"]);
					SubmitTheme(t);
				}
			}
			auto sounds = custom["sounds"];
			if (sounds.is_array())
			{
				for (const auto& sound : sounds)
				{
					if (!sounds.is_object())continue;
					TaskSound s;
					s.custom = true;
					s.name = sound["name"];
					s.filePath = sound["path"];
					SubmitSound(s);
				}
			}
		}
		
		auto settings = json_["settings"];
		if (settings.is_object())
		{
			std::string theme = settings["theme"];
			std::string completed = settings["completed"];
			std::string deleted = settings["deleted"];
			m_selectedTheme = TaskTheme::GetThemeByName(theme).value_or(GetAvailableThemes().at(0));
			m_selectedCompleteSound = TaskSound::GetSoundByName(completed).value_or(GetAvailableSounds().at(0));
			m_selectedDeleteSound = TaskSound::GetSoundByName(deleted).value_or(GetAvailableSounds().at(0));
			if (settings["open"].is_string())
				m_lastOpen = settings["open"];
			if (settings["open_last"].is_boolean())
				m_shouldOpenLast = settings["open_last"];
			if (settings["window"].is_object())
			{
				int w = settings["window"]["w"];
				int h = settings["window"]["h"];
				m_windSize = wxSize(w, h);
			}
			if (settings["ontop"].is_boolean())
				m_ontop = settings["ontop"];
		}
		else 
		{
			createDefaultSettings();
			return false;
		}

		return true;
	}

	inline json objectFromColour(wxColour colour)
	{
		json obj = { colour.GetRed(), colour.GetGreen(), colour.GetBlue() };
		return obj;
	}
	
	inline wxColour colourFromObject(const json& obj)
	{
		return wxColour(obj[0], obj[1], obj[2]);
	}

	bool Settings::save()
	{

		std::ofstream output(m_filePath);
		if (!output)
			return false;

		json json_;
		json customObj = json::object();
		json settings = json::object();

		settings["theme"] = m_selectedTheme.name;
		settings["completed"] = m_selectedCompleteSound.name;
		settings["deleted"] = m_selectedDeleteSound.name;
		settings["open_last"] = m_shouldOpenLast;
		settings["ontop"] = m_ontop;
		settings["window"] = { {"w", m_windSize.x}, {"h", m_windSize.y} };
		if (!m_lastOpen.empty())
		{
			settings["open"] = m_lastOpen;
		}

		json customThemes = json::array();
		for (auto theme : m_customThemes)
		{
			json themeObj = json::object();
			themeObj["name"] = theme.name;
			themeObj["bg"] = objectFromColour(theme.bgColor);
			themeObj["fg"] = objectFromColour(theme.fgColor);
			themeObj["hover"] = objectFromColour(theme.hover);
			themeObj["active"] = objectFromColour(theme.active);
			customThemes.push_back(themeObj);
		}

		json customSounds = json::array();
		for (auto sound : m_customSounds)
		{
			json soundObj = json::object();
			soundObj["name"] = sound.name;
			soundObj["path"] = sound.filePath;
		}

		customObj["themes"] = customThemes;
		customObj["sounds"] = customSounds;
		
		json_["settings"] = settings;
		json_["custom"] = customObj;

		output << json_ << std::endl;
		output.close();
		
		return false;
	}

}