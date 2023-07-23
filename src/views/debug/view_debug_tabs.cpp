/**
 * @file view_debug_tabs.cpp
 * @brief Debug options tab bar.
 */

#include "views/view.hpp"

namespace big
{
	void view::debug_tabs()
	{
		if (ImGui::BeginTabBar("debug_tabbar"))
		{
			if (ImGui::BeginTabItem("Demo"))
			{
				view::debug_demo();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Threads"))
			{
				view::debug_threads();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
}