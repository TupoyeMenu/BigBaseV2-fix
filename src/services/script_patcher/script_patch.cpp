/**
 * @file script_patch.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "script_patch.hpp"

#include "script_data.hpp"
#include <cstdint>

namespace big
{
	script_patch::script_patch(rage::joaat_t script, const memory::pattern pattern, int32_t offset, std::vector<std::uint8_t> patch, bool* enable_bool) :
	    m_script(script),
	    m_pattern(pattern),
	    m_offset(offset),
	    m_patch(std::move(patch)),
	    m_bool(enable_bool),
	    m_ip(0)
	{
	}

	std::uint8_t* script_patch::get_code_address(script_data* data, std::uint32_t index)
	{
		return &data->m_bytecode[index >> 14][index & 0x3FFF];
	}

	const std::optional<uint32_t> script_patch::get_code_location_by_pattern(script_data* data, const memory::pattern& pattern)
	{
		std::uint32_t code_size = data->m_code_size;
		for (std::uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (std::uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *get_code_address(data, i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}

	void script_patch::enable(script_data* data)
	{
		std::memcpy(get_code_address(data, m_ip), m_patch.data(), m_patch.size());
	}

	void script_patch::disable(script_data* data)
	{
		std::memcpy(get_code_address(data, m_ip), m_original.data(), m_original.size());
	}

	void script_patch::update(script_data* data)
	{
		if (m_ip == 0)
		{
			auto result = get_code_location_by_pattern(data, m_pattern);
			if (!result.has_value())
			{
				LOG(FATAL) << "Failed to find pattern for script " << m_script;
				for(std::uint8_t byte : m_patch)
					LOG(VERBOSE) << HEX_TO_UPPER(byte);
			}

			m_ip = result.value() + m_offset;

			for (int i = 0; i < m_patch.size(); i++)
				m_original.push_back(*get_code_address(data, m_ip + i));
		}

		if (!m_bool || *m_bool)
			enable(data);
		else
			disable(data);
	}
}