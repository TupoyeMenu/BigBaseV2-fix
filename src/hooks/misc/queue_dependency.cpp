/**
 * @file queue_dependency.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "hooking.hpp"
#include "pointers.hpp"

#include <psapi.h>

namespace big
{
	bool inline is_address_in_game_region(uint64_t address)
	{
		static uint64_t moduleBase = NULL;
		static uint64_t moduleSize = NULL;
		if ((!moduleBase) || (!moduleSize))
		{
			MODULEINFO info;
			if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info)))
			{
				LOG(FATAL) << "GetModuleInformation failed!";
				return true;
			}
			else
			{
				moduleBase = (uint64_t)GetModuleHandle(0);
				moduleSize = (uint64_t)info.SizeOfImage;
			}
		}
		return address > moduleBase && address < (moduleBase + moduleSize);
	}

	bool is_jump(__int64 fptr)
	{
		if (!is_address_in_game_region(fptr))
			return false;

		auto value = *(std::uint8_t*)(fptr);
		return value == 0xE9;
	}

	bool is_unwanted_dependency(__int64 cb)
	{
		auto f1 = *(__int64*)(cb + 0x60);
		auto f2 = *(__int64*)(cb + 0x100);
		auto f3 = *(__int64*)(cb + 0x1A0);

		if (!is_address_in_game_region(f1) || !is_address_in_game_region(f2) || !is_address_in_game_region(f3))
			return false;

		return is_jump(f1) || is_jump(f2) || is_jump(f3);
	}

	void hooks::queue_dependency(void* dependency)
	{
		if (is_unwanted_dependency((__int64)dependency))
		{
			LOG(INFO) << "Prevent attempt to queue a report to R*.";
			return;
		}

		return g_hooking->get_original<hooks::queue_dependency>()(dependency);
	}
}