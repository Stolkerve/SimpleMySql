#pragma once

#include <memory>
#include <iostream>
#include <vector>

#include <ImGuiColorTextEdit/TextEditor.h>
#include <mysql-connector-c++/include/mysql/jdbc.h>

#include "Window.h"
#include "ImGuiApp.h"

namespace simple
{
	class App
	{
	public:
		App();
		~App();
		
		void Start();
	private:
		void RenderImGui();
		void RenderTextEditor();
		void RenderTables();

		bool ExecutingQuery();

		void ReadSaveFile();
		void WriteSaveFile();
	private:
		Window m_Window;
		ImGuiApp m_ImGuiApp;
		TextEditor m_TextEditor;
		std::string m_SelectedLine;
		bool m_LoginSuccess = false;
		bool m_RetValue = false;
		std::unique_ptr<sql::mysql::MySQL_Driver> m_Driver;
		std::unique_ptr<sql::Connection> m_Connection;
		std::unique_ptr<sql::Statement> m_Stmt;
		sql::ResultSet* m_QueryResult = nullptr;
		std::vector<std::string> m_Columns;
		std::vector<std::string> m_Rows;
		std::vector<std::vector<std::string>> m_Matrix;

		std::string m_User;
		std::string m_Password;
	};
}