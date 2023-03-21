#include "base_tab.h"
#include "fiber_pool.hpp"
#include "imgui.h"
#include "shv_runner.hpp"

#include <libloaderapi.h>
#include <shlwapi.h>
#include <utility>

static std::pair<HMODULE, void (*)()> selected_script;

namespace big
{
	void base_tab::render_open_hook_tab()
	{
		if (ImGui::BeginTabItem("OpenVHook"))
		{
			if (ImGui::ListBoxHeader("##scripts", ImVec2(250, -ImGui::GetFrameHeight())))
			{
				for (auto& iter : big::shv_runner::scripts)
				{
					char dll_name[MAX_PATH] = {0};
					if (GetModuleFileNameA(iter.first, dll_name, MAX_PATH) != 0)
					{
						std::string filepath(dll_name);
						filepath = filepath.substr(filepath.find_last_of('\\') + 1);
						if (ImGui::Selectable(fmt::format("{} - {:X}", filepath.c_str(), (uint64_t)iter.second).c_str(), selected_script.second == iter.second))
						{
							selected_script = iter;
						}
					}
				}
				ImGui::ListBoxFooter();
			}

			if (selected_script.second)
			{
				ImGui::SameLine();
				ImGui::BeginGroup();

				ImGui::Text("HMODULE: 0x%llX", (uint64_t)selected_script.first);
				ImGui::Text("Pointer: 0x%llX", (uint64_t)selected_script.second);
				
				ImGui::EndGroup();
			}

			ImGui::EndTabItem();
		}
	}
}