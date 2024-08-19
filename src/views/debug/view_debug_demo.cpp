/**
 * @file view_debug_demo.cpp
 * @brief Demo window from BigBaseV2
 * Used to demonstrate basic ImGui functionality.
 */

#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "gui/components/components.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "views/view.hpp"
#include "widgets/imgui_bitfield.h"
#include <cstdint>

namespace big
{
	void view::debug_demo()
	{
		const char* const demo_combo[]{"One", "Two", "Three"};
		const double min = 0., max = 10.;

		//If you want to add a new option, you have to first declare it in core/settings.hpp's default_options, otherwise, this code will crash when trying to access an option that does not exist in memory.
		ImGui::Checkbox("Bool", &g.demo.demo_bool);
		ImGui::SliderInt("Int", &g.demo.demo_int, 0, 10);
		ImGui::SliderScalar("Double", ImGuiDataType_Double, &g.demo.demo_double, &min, &max); //JSON does not describe rational numbers as integer/float/double/etc types, it is just "number". See: https://nlohmann.github.io/json/features/types/
		ImGui::Combo("Combo", &g.demo.demo_combo, demo_combo, sizeof(demo_combo) / sizeof(*demo_combo));
		ImGui::Bitfield("Bitfield", &g.demo.demo_bitset);

		components::input_text_with_hint("##Vehicle", "Vehicle model", &g.demo.demo_vehicle_model, ImGuiInputTextFlags_None);

		// `components::button` has a fiber pool wraper inside it.
		components::button("Spawn", [] {
			rage::joaat_t hash = rage::joaat(g.demo.demo_vehicle_model);
			while (!STREAMING::HAS_MODEL_LOADED(hash))
			{
				STREAMING::REQUEST_MODEL(hash);
				script::get_current()->yield();
			}

			*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x9090;
			Vehicle vehicle = VEHICLE::CREATE_VEHICLE(hash, self::pos.x, self::pos.y, self::pos.z, 0.f, TRUE, FALSE, FALSE);
			*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x0574; //By writing the "old" bypass to the function, running CREATE_VEHICLE, then restoring it, the anti-cheat does not have enough time to catch the function in a dirty state.

			script::get_current()->yield();
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
			if (*g_pointers->m_is_session_started)
			{
				DECORATOR::DECOR_SET_INT(vehicle, "MPBitset", 0);
				ENTITY::SET_ENTITY_SHOULD_FREEZE_WAITING_ON_COLLISION(vehicle, TRUE); //True means it can be deleted by the engine when switching lobbies/missions/etc, false means the script is expected to clean it up.
				auto networkId = NETWORK::VEH_TO_NET(vehicle);
				if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(vehicle))
					NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
				VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE);
			}
		});

		if (ImGui::Button("Test g3log crash within ImGui"))
		{
			*((uint64_t*)0) = 0xDEADBEEF;
		}
		ImGui::SameLine();
		if (ImGui::Button("Test g3log crash within GTA V Script"))
		{
			g_fiber_pool->queue_job([] {
				//PED::SPAWNPOINTS_GET_SEARCH_RESULT_FLAGS(PLAYER::PLAYER_PED_ID(), nullptr); //This causes a crash at GTA5.exe+5845356 and nothing of value was in the log in the stack dump because of the context switch to GTA 5's memory. If you encounter something similar, you will have to figure out where the crash occured in the GTA 5 exe, and trace back that native, and figure out which function is calling the native that is crashing.
				*((PINT) nullptr) = 0xDEADBEEF;
			});
		}
	}
}