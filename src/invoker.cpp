/**
 * @file invoker.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "invoker.hpp"

#include "common.hpp"
#include "crossmap.hpp"
#include "pointers.hpp"

namespace big
{
	native_call_context::native_call_context()
	{
		m_return_value = &m_return_stack[0];
		m_args         = &m_arg_stack[0];
	}

	void native_invoker::cache_handlers()
	{
		if (m_handlers_cached)
			return;

		for (const rage::scrNativeMapping& mapping : g_crossmap)
		{
			rage::scrNativeHandler handler =
			    g_pointers->m_get_native_handler(g_pointers->m_native_registration_table, mapping.second);
			m_handler_cache.emplace(mapping.first, handler);
		}

		m_handlers_cached = true;
	}

	void native_invoker::begin_call()
	{
		m_call_context.reset();
	}

	void native_invoker::end_call(rage::scrNativeHash hash)
	{
		if (auto it = m_handler_cache.find(hash); it != m_handler_cache.end())
		{
			rage::scrNativeHandler handler = it->second;

			// return address checks are no longer a thing
			handler(&m_call_context);
			g_pointers->m_fix_vectors(&m_call_context);
		}
		else
		{
			[hash]() {
				LOG(WARNING) << "Failed to find " << HEX_TO_UPPER(hash) << " native's handler.";
			}();
		}
	}
}
