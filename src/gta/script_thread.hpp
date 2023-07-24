/**
 * @file script_thread.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "fwddec.hpp"
#include "joaat.hpp"

#include <cstdint>

class CGameScriptHandlerNetComponent;

class GtaThread : public rage::scrThread
{
public:
	rage::joaat_t m_script_hash;               // 0x120
	char m_padding3[0x14];                     // 0x124
	std::int32_t m_instance_id;                // 0x138
	char m_padding4[0x04];                     // 0x13C
	uint8_t m_flag1;                      // 0x140
	bool m_safe_for_network_game;              // 0x141
	char m_padding5[0x02];                     // 0x142
	bool m_is_minigame_script;                 // 0x144
	char m_padding6[0x02];                     // 0x145
	bool m_can_be_paused;                      // 0x147
	bool m_can_remove_blips_from_other_scripts;// 0x148
	char m_padding7[0x0F];                     // 0x149
};

static_assert(sizeof(GtaThread) == 0x160);
