#include "App.h"

#include "logging/SimpleLogger.h"
#include "Widgets.h"

namespace simple
{
	App::App() :
		m_Window(960, 640, "Simple Request"),
		m_ImGuiApp(m_Window.GetWindow()),
		m_Driver(sql::mysql::get_mysql_driver_instance())
	{
		m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
		m_TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::MySQL());
		m_TextEditor.SetTextLines({ "", "", "", "", "" });
	}

	void App::Start()
	{
		while (!m_Window.isClose())
		{
			m_Window.PollEvents();
			m_ImGuiApp.Begin();

			RenderImGui();

			m_Window.Clear();
			m_ImGuiApp.End();
			m_Window.Display();
		}
	}

	void App::RenderImGui()
	{
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoTabBar;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

		auto viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

			static auto first_time = true;
			if (first_time)
			{
				first_time = false;

				ImGui::DockBuilderRemoveNode(dockspaceID); // clear any previous layout
				ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

				// Split layout
				auto downID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &dockspaceID);

				// build Layout
				ImGui::DockBuilderDockWindow("Center", dockspaceID);
				ImGui::DockBuilderDockWindow("Down", downID);
				ImGui::DockBuilderFinish(dockspaceID);
			}
		}
		ImGui::End();

		// Panels

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 4.0f });

		//ImGui::ShowDemoWindow();
		if (!m_LoginSuccess)
		{
			auto [user, password] = LoginModal();
			if (user.length() > 0 && password.length() > 0)
			{
				try
				{
					m_Connection.reset(m_Driver->connect("tcp://127.0.0.1:3306", user, password));
					m_Stmt.reset(m_Connection->createStatement());
					m_LoginSuccess = true;
				}
				catch (std::exception e)
				{
					Logger::Error(e.what());
				}
			}
		}
		else
		{

			RenderTextEditor();
			RenderTables();
		}

		ImGui::PopStyleVar(2);


		ImGui::Render();
	}

	void App::RenderTextEditor()
	{
		ImGui::Begin("Center", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		m_TextEditor.Render("TextEditor");
		ImGui::End();
	}

	void App::RenderTables()
	{
		ImGui::Begin("Down", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::IsKeyDown(341) && ImGui::IsKeyPressed(257, false))
		{
			if (ExecutingQuery())
			{
				try
				{
					m_QueryResult = m_Stmt->getResultSet();

					auto res_meta = m_QueryResult->getMetaData();
					size_t numOfCols = res_meta->getColumnCount();

					m_Columns.clear();
					m_Rows.clear();

					for (size_t i = 0; i < numOfCols; i++) m_Columns.push_back(res_meta->getColumnLabel(i + 1));
					while (m_QueryResult->next())
						for (size_t i = 0; i < numOfCols; i++)
							m_Rows.push_back(m_QueryResult->getString(i + 1));

					m_QueryResult->close();
					m_RetValue = false;
				}
				catch (std::exception e)
				{
					Logger::Error(e.what());
				}
			}
			else
				m_QueryResult = nullptr;
		}
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;
		if (m_QueryResult)
		{
			if (ImGui::BeginTable("table", m_Columns.size(), flags))
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
				for (auto& c : m_Columns)
					ImGui::TableSetupColumn(c.c_str(), ImGuiTableColumnFlags_WidthFixed);

				ImGui::TableHeadersRow();
				for (size_t i = 0; i < m_Rows.size() / m_Columns.size(); i++)
				{
					ImGui::TableNextRow();
					for (size_t e = 0; e < m_Columns.size(); e++)
					{
						size_t index = (i * m_Columns.size()) + e;
						ImGui::TableSetColumnIndex(e);
						auto& value = m_Rows[index];
						ImGui::Text((value.compare("") ? value : "null").c_str());
					}
				}
				ImGui::EndTable();
			}
		}

		ImGui::End();
	}

	bool App::ExecutingQuery()
	{
		auto line = m_TextEditor.GetCurrentLineText();
		auto linePos = m_TextEditor.GetCursorPosition().mLine + 1;

		if (line.length() > 0)
		{
			line.replace(std::remove_if(line.begin(), line.end(), [&](const char c) { return (c == '\n'); }), line.end(), "");
			auto dotStickPos = line.find(';');
			if (dotStickPos == std::string::npos)
			{
				TextEditor::ErrorMarkers error;
				error.insert({ linePos, "Semicolon missing" });
				m_TextEditor.SetErrorMarkers(error);
			}
			else
			{
				m_TextEditor.SetErrorMarkers({});
				try
				{
					return m_Stmt->execute(line); // the query return a value?
				}
				catch (std::exception e)
				{
					TextEditor::ErrorMarkers error;
					error.insert({ linePos, e.what() });
					m_TextEditor.SetErrorMarkers(error);
					Logger::Error(e.what());
					return false;
				}
			}
		}

		return false;
	}
}