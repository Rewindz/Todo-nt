#pragma once

#include <optional>
#include <string>
#include <nlohmann/json.hpp>

#include "TaskUtils.hpp"

namespace ToDont
{

	inline wxColour colourFromObject(const nlohmann::json& obj);
	inline nlohmann::json objectFromColour(wxColour colour);

	class Settings
	{
	public:
		Settings(std::string filePath = "./settings.json");

		~Settings();

		bool load();
		bool save();

		void SubmitTheme(TaskTheme theme);
		void SubmitSound(TaskSound sound);

		TaskSound GetCompleteSound() const { return m_selectedCompleteSound; }
		void SetCompleteSound(TaskSound sound) { m_selectedCompleteSound = sound; if (m_updatedCb) m_updatedCb(); }
		TaskSound GetDeleteSound() const  { return m_selectedDeleteSound; }
		void SetDeleteSound(TaskSound sound) { m_selectedDeleteSound = sound; if (m_updatedCb) m_updatedCb(); }
		TaskTheme GetTheme() const { return m_selectedTheme; }
		void SetTheme(TaskTheme theme) { m_selectedTheme = theme; if (m_updatedCb) m_updatedCb(); }
		std::optional<std::string> GetLastOpen() const;
		void SetLastOpen(std::string path) { m_lastOpen = path; }
		bool GetShouldOpenLast() const { return m_shouldOpenLast; }
		void SetShouldOpenLast(bool should) { m_shouldOpenLast = should; }
		std::string GetSaveFilePath() const { return m_filePath; }
		void SetSaveFilePath(std::string path) { m_filePath = path; }
		wxSize GetWindowSize() const { return m_windSize; }
		void SetWindowSize(wxSize size) { m_windSize = size; }
		void SetWidth(int w) { m_windSize.SetWidth(w); }
		void SetHeight(int h) { m_windSize.SetHeight(h); }
		bool GetOntop() const { return m_ontop; }
		void SetOntop(bool ontop) { m_ontop = ontop; }

		void SetUpdatedCallback(std::function<void()> callback) { m_updatedCb = std::move(callback); }

	private:

		void createDefaultSettings();

		std::string m_filePath; // Settings savefile path.
		TaskSound m_selectedCompleteSound;
		TaskSound m_selectedDeleteSound;
		TaskTheme m_selectedTheme;
		std::string m_lastOpen;
		std::vector<TaskTheme> m_customThemes;
		std::vector<TaskSound> m_customSounds;
		wxSize m_windSize;
		bool m_shouldOpenLast;
		bool m_ontop;
		std::function<void()> m_updatedCb;

	};
}