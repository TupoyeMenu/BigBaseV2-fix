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

			ImGui::EndTabBar();
		}
	}
}