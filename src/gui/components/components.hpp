/**
 * @file components.hpp
 * @brief YimMenu wrapers for ImGui functions.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "fiber_pool.hpp"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace big
{
	class components
	{
	public:
		static bool menu_item(const std::string_view);
		static void menu_item(const std::string_view, std::function<void()>);
		static void help_marker(const std::string_view);

		static void input_text_with_hint(const std::string_view label, const std::string_view hint, char* buf, size_t buf_size, ImGuiInputTextFlags flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);
		static void input_text_with_hint(const std::string_view label, const std::string_view hint, std::string* buf, ImGuiInputTextFlags flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);
		static void input_text(const std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags_ flag = ImGuiInputTextFlags_None, std::function<void()> cb = nullptr);

		template<ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.172f, 0.380f, 0.909f, 1.f)> // TODO: Use GUI Color.
		static bool button(const std::string_view text)
		{
			bool status = false;

			if constexpr (color.x != 0.172f || color.y != 0.380f || color.z != 0.909f || color.w != 1.0f)
				ImGui::PushStyleColor(ImGuiCol_Button, color);

			status = ImGui::Button(text.data(), size);

			if constexpr (color.x != 0.172f || color.y != 0.380f || color.z != 0.909f || color.w != 1.0f)
				ImGui::PopStyleColor(1);
			return status;
		}

		template<ImVec2 size = ImVec2(0, 0), ImVec4 color = ImVec4(0.172f, 0.380f, 0.909f, 1.f)> // TODO: Use GUI Color.
		static void button(const std::string_view text, std::function<void()> cb)
		{
			if (button<size, color>(text))
			{
				g_fiber_pool->queue_job(cb);
			}
		}
	};
}