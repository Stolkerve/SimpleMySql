#include "Widgets.h"
#include <imgui/imgui.h>

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

ImVec2 CalButtomSize(std::string_view label)
{
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 label_size = ImGui::CalcTextSize(label.data(), NULL, true);
    return ImGui::CalcItemSize({ 0.f, 0.f }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
}

bool LoginModal(std::string& lastUser, std::string& lastPassword)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5.0f, 5.0f });
    ImGui::OpenPopup("LoginModal");

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("LoginModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // ImGuiInputTextFlags_Password
        // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        //static std::string user = lastUser.length() > 0 ? lastUser : "";
        //static std::string password = lastPassword.length() > 0 ? lastPassword : "";
        ImGui::Text("Username");
        InputText("##username", &lastUser);

        ImGui::Text("Password");
        InputText("##password", &lastPassword, ImGuiInputTextFlags_Password);

        if (ImGui::Button("Submit", ImVec2(120, 0)))
        {
            //UserDetails tempUserDetails = { user, password };
            //user.clear();
            //password.clear();
            ImGui::CloseCurrentPopup();
            ImGui::PopStyleVar();
            ImGui::EndPopup();
            return true;
        }
        
        if (ImGui::Button("Instructions"))
            ImGui::OpenPopup("InstructionsModal");

        if (ImGui::BeginPopupModal("InstructionsModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BulletText("Press ctrl+enter to execute a query");
            ImGui::BulletText("Only support single line query (For now)");
            ImGui::BulletText("Host in the localhost and the default port");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    return false;
}
