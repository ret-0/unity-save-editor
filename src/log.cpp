// log.cpp - Logging utilites.

#include "log.hpp"

#include "../imgui/imgui.h"

void InfoChar() { ImGui::Text("[-] "); };
void SuccessChar() {
	ImGui::Text("[");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
	ImGui::Text("*");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PopStyleColor();
	ImGui::Text("] ");
	ImGui::SameLine(0.0F, 0.0F);
};
void WarningChar() {
	ImGui::Text("[");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 100, 0, 255));
	ImGui::Text("#");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PopStyleColor();
	ImGui::Text("] ");
	ImGui::SameLine(0.0F, 0.0F);
};
void ErrorChar() {
	ImGui::Text("[");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
	ImGui::Text("!");
	ImGui::SameLine(0.0F, 0.0F);
	ImGui::PopStyleColor();
	ImGui::Text("] ");
	ImGui::SameLine(0.0F, 0.0F);
};
void DebugChar() {
	ImGui::Text("[?] ");
};

namespace logger
{
	vector<log_t> logs;

	void DrawLatestLog()
	{
		if (logs.empty()) return;

		log_t log = logs.back();
		switch (log.type) {
		case logger::INFO: InfoChar(); break;
		case logger::SUCCESS: SuccessChar(); break;
		case logger::WARNING: WarningChar(); break;
		case logger::ERROR: ErrorChar(); break;
		case logger::DEBUG: DebugChar(); break;
		}
		ImGui::SameLine(0.0F, 0.0F);
		ImGui::Text("%s", log.message.c_str());
	}

	void DrawLogs(string sourceFilter)
	{
		for (log_t entry : logs) {
			if (sourceFilter == "" || entry.source.compare(sourceFilter) == 0) {
				switch (entry.type) {
				case logger::INFO: InfoChar(); break;
				case logger::SUCCESS: SuccessChar(); break;
				case logger::WARNING: WarningChar(); break;
				case logger::ERROR: ErrorChar(); break;
				case logger::DEBUG: DebugChar(); break;
				}
				ImGui::SameLine(0.0F, 0.0F);
	
				ImGui::Text("%s", entry.message.c_str());
			}
		}
	}
	
	void Info(string source, string message)
	{
		log_t entry;
		entry.type = logger::INFO;
		entry.source = source;
		entry.message = message;
		logs.push_back(entry);
		
		pop::Info(message);
	}

	void Success(string source, string message)
	{
		log_t entry;
		entry.type = logger::SUCCESS;
		entry.source = source;
		entry.message = message;
		logs.push_back(entry);

		pop::Success(message);
	}

	void Warning(string source, string message)
	{
		log_t entry;
		entry.type = logger::WARNING;
		entry.source = source;
		entry.message = message;
		logs.push_back(entry);
		
		pop::Warning(message);
	}

	void Error(string source, string message)
	{
		log_t entry;
		entry.type = logger::ERROR;
		entry.source = source;
		entry.message = message;
		logs.push_back(entry);
		
		pop::Error(message);
	}

	void Debug(string source, string message)
	{
		log_t entry;
		entry.type = logger::DEBUG;
		entry.source = source;
		entry.message = message;
		logs.push_back(entry);
		
		pop::Debug(message);
	}

	void Fatal(string source, string message)
	{
		pop::Fatal(message);
		exit(1);
	}
}
