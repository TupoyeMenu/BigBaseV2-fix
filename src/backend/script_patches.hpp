/**
 * @file script_patches.hpp
 *
 * @brief Patches to ysc scripts the game runs.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "pointers.hpp"
#include "services/script_patcher/script_patcher_service.hpp"
#include "gta/joaat.hpp"

#include <script/scrProgramTable.hpp>

namespace big
{
	void register_script_patches()
	{
		/**
		 * @brief Prevents DLC vehicles from despawning in SP, dripfeed vehicles from despawning in online.
		 * @details Function hash: 0xB688D6D
		 * @details The only instance of `NETWORK::NETWORK_WAS_GAME_SUSPENDED()` in the script.
		 *
		 * @return PUSH_CONST_1; LEAVE 1, 1
		 */
		g_script_patcher_service->add_patch({RAGE_JOAAT("shop_controller"), "2D 01 04 00 00 2C ? ? ? 56 ? ? 71", 5, {0x71, 0x2E, 0x01, 0x01}, nullptr});

		/**
		 * @brief Godmode/Invisibility detection bypass.
		 * @details Function hash: 0xC0E97F83
		 * @details Has `NETSHOPPING::NET_GAMESERVER_CHECKOUT_START(*piParam0)`
		 */
		g_script_patcher_service->add_patch({RAGE_JOAAT("shop_controller"), "38 00 5D ? ? ? 38 00 5D ? ? ? 38 00 41", 0, std::vector<uint8_t>(12, 0x0), nullptr});

		for (auto& entry : *g_pointers->m_script_program_table)
		{
			if (entry.m_program)
				g_script_patcher_service->on_script_load(entry.m_program);
		}
	}
}