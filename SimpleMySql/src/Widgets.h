#pragma once

#include <map>
#include <string>
#include <string_view>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

bool LoginModal(std::string& lastUser, std::string& lastPassword);

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

ImVec2 CalButtomSize(std::string_view label);