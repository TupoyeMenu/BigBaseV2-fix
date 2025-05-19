/**
 * @file byte_patch_manager.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "byte_patch_manager.hpp"

#include "memory/byte_patch.hpp"
#include "pointers.hpp"

namespace big
{
	static void init()
	{
		// Patch script network check
		if (!g_is_enhanced) // YimMenuV2 also has this patch, but doesn't apply it for some reason.
			memory::byte_patch::make(g_pointers->m_model_spawn_bypass, std::vector{0x90, 0x90})->apply(); // this is no longer integrity checked
	}

	byte_patch_manager::byte_patch_manager()
	{
		init();

		g_byte_patch_manager = this;
	}

	byte_patch_manager::~byte_patch_manager()
	{
		memory::byte_patch::restore_all();

		g_byte_patch_manager = nullptr;
	}
}