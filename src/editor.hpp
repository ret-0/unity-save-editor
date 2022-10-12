// editor.hpp - Editor window.

#include <fstream>

#include "../imgui/ImGuiFileDialog/ImGuiFileDialog.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

static json j = nullptr;
static bool modified = false;

#define INPUT_SCALAR(type, dataType) \
{ \
	type * i = new type; \
	*i = value.get<type>(); \
	if (ImGui::InputScalar("##", dataType, i)) { \
		modified = true; \
		if (j[name].back().is_array()) j[name].back()[index] = *i; \
		else j[name].back() = *i; \
	} \
	delete i; \
}

string Run(const char* cmd)
{
	FILE* pipe = popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");

	string result;
	static char buffer[1024];
	while (!feof(pipe)) {
		if (fgets(buffer, 1024, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);

	return result;
}

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void DisplayValue(const char* name, const char* type, json& value, int index)
{
	if (index != -1) ImGui::PushID(index);

	// This should really be a hash table. TOO BAD!
	if      (strcmp(type, "System.Byte") == 0)    { INPUT_SCALAR(uint8_t,  ImGuiDataType_U8); }
	else if (strcmp(type, "System.SByte") == 0)   { INPUT_SCALAR(int8_t,   ImGuiDataType_S8); }
	else if (strcmp(type, "System.Char") == 0)    { INPUT_SCALAR(int8_t,   ImGuiDataType_S8); }
	else if (strcmp(type, "System.Int16") == 0)   { INPUT_SCALAR(int16_t,  ImGuiDataType_S16); }
	else if (strcmp(type, "System.UInt16") == 0)  { INPUT_SCALAR(uint16_t, ImGuiDataType_U16); }
	else if (strcmp(type, "System.Int32") == 0)   { INPUT_SCALAR(int32_t,  ImGuiDataType_S32); }
	else if (strcmp(type, "System.UInt32") == 0)  { INPUT_SCALAR(uint32_t, ImGuiDataType_U32); }
	else if (strcmp(type, "System.Int64") == 0)   { INPUT_SCALAR(int64_t,  ImGuiDataType_S64); }
	else if (strcmp(type, "System.UInt64") == 0)  { INPUT_SCALAR(uint64_t, ImGuiDataType_U64); }
	else if (strcmp(type, "System.Single") == 0)  { INPUT_SCALAR(float,    ImGuiDataType_Float); }
	else if (strcmp(type, "System.Double") == 0)  { INPUT_SCALAR(double,   ImGuiDataType_Double); }
	else if (strcmp(type, "System.Decimal") == 0) { INPUT_SCALAR(double,   ImGuiDataType_Double); } // Technically should be a 128-bit floating point value.
	else if (strcmp(type, "System.Boolean") == 0) {
		bool state = value.get<bool>();
		if (state) {
			if (ImGui::Button("true")) {
				modified = true;
				// Operator Overloading was probably a mistake
				if (j[name].back().is_array()) j[name].back()[index] = false;
				else j[name].back() = false;
			}
		} else {
			if (ImGui::Button("false")) {
				modified = true;
				if (j[name].back().is_array()) j[name].back()[index] = true;
				else j[name].back() = true;
			}
		}
	} else if (strcmp(type, "System.String") == 0) {
		string s = value.get<string>();
		if (ImGui::InputText("##", &s)) {
			modified = true;
			if (j[name].back().is_array()) j[name].back()[index] = s;
			else j[name].back() = s;
		}
	}

	if (index != -1) ImGui::PopID();
}

void Editor()
{
	// PATH_MAX
	static char assemblyPath[4096];
	static char savePath[4096];
	const ImVec2 filePickerSize = ImVec2(600, 300);

	if (ImGui::Button("Open##Assembly")) {
		ImGuiFileDialog::Instance()->OpenDialog("AssemblyPath", "Assembly-CSharp.dll", ".dll", (strlen(assemblyPath) == 0) ? "." : assemblyPath);
		ImGui::SetNextWindowSize(filePickerSize);
	}
	ImGui::SameLine();
	ImGui::InputText("Assembly-CSharp.dll Path", assemblyPath, sizeof (assemblyPath));
	ImGui::SameLine();
	HelpMarker("Assembly-CSharp.dll typically resides in GameFolder/Game_Data/Managed/");
	if (ImGuiFileDialog::Instance()->Display("AssemblyPath")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			string s = ImGuiFileDialog::Instance()->GetFilePathName();
			memset(assemblyPath, 0x00, sizeof (assemblyPath));
			memcpy(assemblyPath, s.c_str(), s.length());
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Open##Save")) {
		ImGuiFileDialog::Instance()->OpenDialog("SavePath", "Save File", ".*", (strlen(savePath) == 0) ? "." : savePath);
		ImGui::SetNextWindowSize(filePickerSize);
	}
	ImGui::SameLine();
	ImGui::InputText("Save File Path", savePath, sizeof (savePath));
	if (ImGuiFileDialog::Instance()->Display("SavePath")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			string s = ImGuiFileDialog::Instance()->GetFilePathName();
			memset(savePath, 0x00, sizeof (savePath));
			memcpy(savePath, s.c_str(), s.length());
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Load")) {
		string cmdline = "decoder/bin/Release/net6.0/linux-x64/publish/decoder r '" + string(assemblyPath) + "' '" + string(savePath) + "'";
		string s = Run(cmdline.c_str());
		puts(s.c_str());
		if (s[0] == '{') {
			logger::Success("editor", "Loaded " + pop::BaseName(savePath) + "!");
			j = json::parse(s);
		} else logger::Error("editor", "Error loading Assembly-CSharp.dll or Save File! Are the paths correct?");
	}
	ImGui::SameLine();
	if (!modified) ImGui::BeginDisabled();
	if (ImGui::Button("Save")) {
		std::fstream jsonFile("/tmp/save-editor-json", jsonFile.in | jsonFile.out | jsonFile.trunc);
		jsonFile << j.dump(2);
		string cmdline = "decoder/bin/Release/net6.0/linux-x64/publish/decoder w '" + string(assemblyPath) + "' '" + string(savePath) + "' '/tmp/save-editor-json'";
		string s = Run(cmdline.c_str());
		puts(s.c_str());
	}
	if (!modified) ImGui::EndDisabled();
	ImGui::SameLine();
	logger::DrawLatestLog();
	ImGui::Separator();

	if (j != nullptr) {
		ImGui::BeginTable("Memory Map Table", 2,
		                  ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
		                  ImGuiTableFlags_ScrollX  | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp);
		ImGui::TableSetupColumn("Object");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		size_t i = 0;
		for (json::iterator element = j.begin(); element != j.end(); element++) {
			ImGui::TableNextRow();
			ImGui::PushID(i);

			const char* name = element.key().c_str();
			json pair = element.value();
			string stype;
			pair.at(0).get_to(stype);
			const char* type = stype.c_str();
			json value = pair.at(1).get<json>();

			if (ImGui::TableSetColumnIndex(0)) {
				string s = type;
				if (s.find("[]") != string::npos) {
					s.pop_back();
					s.pop_back();
					if (ImGui::TreeNodeEx("##", 0, "%s %s", type, name)) {
						int n = 0;
						for (json::iterator element = value.begin(); element != value.end(); element++) {
							ImGui::TableNextRow();

							const char* type = s.c_str();
							json value = element.value();

							if (ImGui::TableSetColumnIndex(0)) ImGui::TreeNodeEx("##", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "%s %s[%d]", type, name, n);
							if (ImGui::TableSetColumnIndex(1)) DisplayValue(name, type, value, n);
							n++;
						}
						ImGui::TreePop();
					}
				} else ImGui::Text("%s %s", type, name);
			}
			if (ImGui::TableSetColumnIndex(1)) DisplayValue(name, type, value, -1);

			ImGui::PopID();
			i++;
		}

		ImGui::EndTable();
	}
}
