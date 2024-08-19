/**
 * @file gui.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gui.hpp"

#include "natives.hpp"
#include "renderer.hpp"
#include "script.hpp"
#include "views/view.hpp"

#include <imgui.h>

namespace big
{
	gui::gui() :
	    m_is_open(false),
	    m_override_mouse(false)
	{
		g_renderer->add_dx_callback(
		    [this] {
			    dx_on_tick();
		    },
		    -1);

		g_renderer->add_wndproc_callback([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			wndproc(hwnd, msg, wparam, lparam);
		});

		dx_init();

		g_gui = this;
	}

	gui::~gui()
	{
		g_gui = nullptr;
	}

	bool gui::is_open()
	{
		return m_is_open;
	}

	void gui::toggle(bool toggle)
	{
		m_is_open = toggle;

		toggle_mouse();
	}

	void gui::override_mouse(bool override)
	{
		m_override_mouse = override;

		toggle_mouse();
	}

	void gui::dx_init()
	{
		auto& style                  = ImGui::GetStyle();
		style.WindowPadding          = {10.f, 10.f};
		style.PopupRounding          = 0.f;
		style.FramePadding           = {8.f, 4.f};
		style.ItemSpacing            = {10.f, 8.f};
		style.ItemInnerSpacing       = {6.f, 6.f};
		style.TouchExtraPadding      = {0.f, 0.f};
		style.IndentSpacing          = 21.f;
		style.ScrollbarSize          = 15.f;
		style.GrabMinSize            = 8.f;
		style.WindowBorderSize       = 1.f;
		style.ChildBorderSize        = 0.f;
		style.PopupBorderSize        = 1.f;
		style.FrameBorderSize        = 0.f;
		style.TabBorderSize          = 0.f;
		style.WindowRounding         = 0.f;
		style.ChildRounding          = 0.f;
		style.FrameRounding          = 0.f;
		style.ScrollbarRounding      = 0.f;
		style.GrabRounding           = 0.f;
		style.TabRounding            = 0.f;
		style.WindowTitleAlign       = {0.5f, 0.5f};
		style.ButtonTextAlign        = {0.5f, 0.5f};
		style.DisplaySafeAreaPadding = {3.f, 3.f};

		auto& colors                           = style.Colors;
		colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled]          = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
		colors[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border]                = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
		colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg]               = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_FrameBgActive]         = ImVec4(0.28f, 0.27f, 0.27f, 0.54f);
		colors[ImGuiCol_TitleBg]               = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_TitleBgActive]         = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.39f, 0.38f, 0.38f, 1.00f);
		colors[ImGuiCol_Button]                = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
		colors[ImGuiCol_ButtonHovered]         = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
		colors[ImGuiCol_ButtonActive]          = ImVec4(0.41f, 0.41f, 0.41f, 0.87f);
		colors[ImGuiCol_Header]                = ImVec4(0.37f, 0.37f, 0.37f, 0.31f);
		colors[ImGuiCol_HeaderHovered]         = ImVec4(0.38f, 0.38f, 0.38f, 0.37f);
		colors[ImGuiCol_HeaderActive]          = ImVec4(0.37f, 0.37f, 0.37f, 0.51f);
		colors[ImGuiCol_Separator]             = ImVec4(0.38f, 0.38f, 0.38f, 0.50f);
		colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.46f, 0.46f, 0.46f, 0.50f);
		colors[ImGuiCol_SeparatorActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.64f);
		colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_Tab]                   = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabHovered]            = ImVec4(0.27f, 0.27f, 0.27f, 0.86f);
		colors[ImGuiCol_TabActive]             = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
		colors[ImGuiCol_TabUnfocused]          = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void gui::dx_on_tick()
	{
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		if (m_is_open)
		{
			view::root(); // frame bg
		}
	}

	void gui::script_on_tick()
	{
		if (g_gui->m_is_open || g_gui->m_override_mouse)
		{
			for (uint8_t i = 0; i <= 6; i++)
				PAD::DISABLE_CONTROL_ACTION(2, i, true);
			PAD::DISABLE_CONTROL_ACTION(2, 106, true);
			PAD::DISABLE_CONTROL_ACTION(2, 329, true);
			PAD::DISABLE_CONTROL_ACTION(2, 330, true);

			PAD::DISABLE_CONTROL_ACTION(2, 14, true);
			PAD::DISABLE_CONTROL_ACTION(2, 15, true);
			PAD::DISABLE_CONTROL_ACTION(2, 16, true);
			PAD::DISABLE_CONTROL_ACTION(2, 17, true);
			PAD::DISABLE_CONTROL_ACTION(2, 24, true);
			PAD::DISABLE_CONTROL_ACTION(2, 69, true);
			PAD::DISABLE_CONTROL_ACTION(2, 70, true);
			PAD::DISABLE_CONTROL_ACTION(2, 84, true);
			PAD::DISABLE_CONTROL_ACTION(2, 85, true);
			PAD::DISABLE_CONTROL_ACTION(2, 99, true);
			PAD::DISABLE_CONTROL_ACTION(2, 92, true);
			PAD::DISABLE_CONTROL_ACTION(2, 100, true);
			PAD::DISABLE_CONTROL_ACTION(2, 114, true);
			PAD::DISABLE_CONTROL_ACTION(2, 115, true);
			PAD::DISABLE_CONTROL_ACTION(2, 121, true);
			PAD::DISABLE_CONTROL_ACTION(2, 142, true);
			PAD::DISABLE_CONTROL_ACTION(2, 241, true);
			PAD::DISABLE_CONTROL_ACTION(2, 261, true);
			PAD::DISABLE_CONTROL_ACTION(2, 257, true);
			PAD::DISABLE_CONTROL_ACTION(2, 262, true);
			PAD::DISABLE_CONTROL_ACTION(2, 331, true);
		}
	}

	void gui::script_func()
	{
		while (true)
		{
			g_gui->script_on_tick();
			script::get_current()->yield();
		}
	}

	void gui::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == g.settings.hotkeys.menu_toggle)
		{
			//Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui->m_is_open)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			toggle(g.settings.hotkeys.editing_menu_toggle || !m_is_open);
			if (g.settings.hotkeys.editing_menu_toggle)
				g.settings.hotkeys.editing_menu_toggle = false;
		}
	}

	void gui::toggle_mouse()
	{
		if (g_gui->m_is_open || g_gui->m_override_mouse)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
	}
}
