#include "StageManager.h"
#include "imgui.h"

void StageManager::Initialize()
{
}

void StageManager::Update()
{
}

void StageManager::InitJson()
{
}

void StageManager::SelectStage()
{
#ifdef _DEBUG

	ImGui::Begin("stage");
    if (ImGui::Button("DEFAULT Camera")) {
    }
    if (ImGui::Button("Follow Camera")) {
    }
    if (ImGui::Button("Top-Down Camera")) {
    }
    if (ImGui::Button("FPS Camera")) {
    }
	/*const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	static int listbox_item_current = 1;
	ImGui::ListBox("listbox", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);*/
	ImGui::End();
#endif // _DEBUG

}
