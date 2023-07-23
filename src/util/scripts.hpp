/**
 * @file scripts.hpp
 * @brief Functions related to GTA 5 .ysc scipts.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "fiber_pool.hpp"
#include "gta/script_handler.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "script_local.hpp"
#include "memory/pattern.hpp"

#include <script/scrProgram.hpp>

namespace big::scripts
{
	inline bool is_loaded(int hash)
	{
		return SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(hash);
	}

	inline bool is_running(int hash)
	{
		return SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash) > 0;
	}

	inline void request_script(int hash)
	{
		SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(hash);
	}

	inline int start_script_with_args(int hash, int* args, int arg_count, int stack_size)
	{
		int thread_id = SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS(hash, args, arg_count, stack_size);
		SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);
		return thread_id;
	}

	inline int start_script(int hash, int stack_size)
	{
		int thread_id = SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH(hash, stack_size);
		SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);
		return thread_id;
	}

	inline bool wait_till_loaded(int hash)
	{
		if (is_loaded(hash))
			return true;
		for (int i = 0; i < 150 && !is_loaded(hash); i++)
			script::get_current()->yield(10ms);
		if (is_loaded(hash))
			return true;
		return false;
	}

	inline bool wait_till_running(int hash)
	{
		if (is_running(hash))
			return true;
		for (int i = 0; i < 150 && !is_running(hash); i++)
			script::get_current()->yield(10ms);
		if (is_running(hash))
			return true;
		return false;
	}

	inline const std::optional<uint32_t> get_code_location_by_pattern(rage::scrProgram* program, const memory::pattern& pattern)
	{
		std::uint32_t code_size = program->m_code_size;
		for (std::uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (std::uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *program->get_code_address(i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}

	// we can't use the script patch service for this
	inline void patch_script(rage::scrProgram* program, std::optional<std::uint32_t> location, std::vector<std::uint8_t> patch, int offset)
	{
		std::uint8_t* bytearray = patch.data();
		if (location)
			memcpy(program->get_code_address(location.value() + offset), bytearray, patch.size());
	}
}