/**
 * @file view_debug_threads.cpp
 * @brief In game .ysc script thread monitor.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "core/data/all_script_names.hpp"
#include "core/data/stack_sizes.hpp"
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "gta/script_handler.hpp"
#include "gta/script_thread.hpp"
#include "gta_util.hpp"
#include "gui/components/components.hpp"
#include "natives.hpp"
#include "network/Network.hpp"
#include "script.hpp"
#include "script_global.hpp"
#include "util/scripts.hpp"
#include "views/view.hpp"

static GtaThread* selected_thread;

static int selected_stack_size             = 128;
static int free_stacks                     = -1;
static const char* selected_stack_size_str = "MULTIPLAYER_MISSION";
static const char* selected_script         = "<SELECT>";

static std::chrono::high_resolution_clock::time_point last_stack_update_time{};

namespace
{
	static void update_free_stacks_count()
	{
		free_stacks = MISC::GET_NUMBER_OF_FREE_STACKS_OF_THIS_SIZE(selected_stack_size);
	}
}

namespace big
{
	void view::debug_threads()
	{
		if (!g_pointers->m_script_threads)
		{
			selected_thread = nullptr;
			return;
		}

		if (ImGui::BeginListBox("##threads", ImVec2(250, -ImGui::GetFrameHeight())))
		{
			for (auto script : *g_pointers->m_script_threads)
			{
				if (script)
				{
					if (script->m_context.m_state != rage::eThreadState::killed && script->m_context.m_stack_size == 0)
						continue;

					ImGui::PushID(script->m_context.m_thread_id);

					if (script->m_context.m_state == rage::eThreadState::killed)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.f));

					if (ImGui::Selectable(script->m_name, selected_thread == script))
					{
						selected_thread = script;
					}

					if (selected_thread == script)
						ImGui::SetItemDefaultFocus();

					if (script->m_context.m_state == rage::eThreadState::killed)
						ImGui::PopStyleColor();

					ImGui::PopID();
				}
			}
			ImGui::EndListBox();
		}

		ImGui::SameLine();
		ImGui::BeginGroup();

		if (selected_thread)
		{
			ImGui::Combo("State", (int*)&selected_thread->m_context.m_state, "RUNNING\0WAITING\0KILLED\0PAUSED\0STATE_4\0");

			if (CGameScriptHandlerNetComponent* net_component = (CGameScriptHandlerNetComponent*)selected_thread->m_net_component)
				if (CScriptParticipant* host = net_component->m_host)
					if (CNetGamePlayer* host_net_player = host->m_net_game_player)
						ImGui::Text("Host: %s", host_net_player->get_name());

			ImGui::Text("m_safe_for_network_game: %s", selected_thread->m_safe_for_network_game ? "Yes" : "No");
			ImGui::Text("m_can_be_paused: %s", selected_thread->m_can_be_paused ? "Yes" : "No");
			ImGui::Text("Stack Pointer / Stack Size %d/%d",
			    selected_thread->m_context.m_stack_pointer,
			    selected_thread->m_context.m_stack_size);
			ImGui::Text("IP: %X", selected_thread->m_context.m_instruction_pointer);
			if (selected_thread->m_context.m_state == rage::eThreadState::killed)
				ImGui::Text("Exit Reason: %s", selected_thread->m_exit_message);

			if (ImGui::Button("Kill"))
			{
				if (selected_thread->m_context.m_stack_size != 0)
					selected_thread->kill();

				selected_thread->m_context.m_state = rage::eThreadState::killed;
			}
		}

		ImGui::Text("Start New Script");

		if (ImGui::BeginCombo("Script", selected_script))
		{
			for (auto script : all_script_names)
			{
				if (ImGui::Selectable(script, script == selected_script))
				{
					selected_script = script;
				}

				if (script == selected_script)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Stack Size", selected_stack_size_str))
		{
			for (auto& p : stack_sizes)
			{
				if (ImGui::Selectable(fmt::format("{} ({})", p.first, p.second).data(), selected_stack_size == p.second))
				{
					selected_stack_size_str = fmt::format("{} ({})", p.first, p.second).data();
					selected_stack_size     = p.second;

					g_fiber_pool->queue_job([] {
						update_free_stacks_count();
					});
				}

				if (p.second == selected_stack_size)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Free Stacks: %d", free_stacks);

		components::button("Start", [] {
			auto hash = rage::joaat(selected_script);

			if (!SCRIPT::DOES_SCRIPT_WITH_NAME_HASH_EXIST(hash))
			{
				return;
			}

			while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(hash))
			{
				SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(hash);
				script::get_current()->yield();
			}

			SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH(hash, selected_stack_size);

			SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);

			update_free_stacks_count();
		});

		ImGui::EndGroup();

		if (*g_pointers->m_game_state != eGameState::Invalid && std::chrono::high_resolution_clock::now() - last_stack_update_time > 100ms)
		{
			last_stack_update_time = std::chrono::high_resolution_clock::now();
			g_fiber_pool->queue_job([] {
				update_free_stacks_count();
			});

			if (*g_pointers->m_game_state != eGameState::Invalid && std::chrono::high_resolution_clock::now() - last_stack_update_time > 100ms)
			{
				last_stack_update_time = std::chrono::high_resolution_clock::now();
				g_fiber_pool->queue_job([] {
					update_free_stacks_count();
				});
			}
		}
	}
}
