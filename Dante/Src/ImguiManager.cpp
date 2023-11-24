#include "Pch.h"
#include "ImguiManager.h"
#include "../ThirdParty/imgui/imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//ImGui::SetCurrentContext(imguiCntx);
	ImGui::StyleColorsDark();
	//ImGui::GetStyle().ScaleAllSizes(1.5f);
	//ImGui::GetIO().FontGlobalScale = 1.4f;
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}

