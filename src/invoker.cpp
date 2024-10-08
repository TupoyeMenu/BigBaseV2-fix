/**
 * @file invoker.cpp
 */

#include "invoker.hpp"

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

		for (const rage::scrNativePair& mapping : g_crossmap)
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
