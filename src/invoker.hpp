/**
 * @file invoker.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "crossmap.hpp"
#include <script/scrNativeHandler.hpp>

namespace big
{
	class native_call_context : public rage::scrNativeCallContext
	{
	public:
		native_call_context();

	private:
		uint64_t m_return_stack[10];
		uint64_t m_arg_stack[100];
	};

	class native_invoker
	{
	public:
		explicit native_invoker() = default;
		~native_invoker()         = default;

		void cache_handlers();
		rage::scrNativeHandler get_native_handler(rage::scrNativeHash hash);

		void begin_call();
		void end_call(rage::scrNativeHash hash);

		template<typename T>
		void push_arg(T&& value)
		{
			m_call_context.push_arg(std::forward<T>(value));
		}

		template<typename T>
		T& get_return_value()
		{
			return *m_call_context.get_return_value<T>();
		}

		void* get_return_address()
		{
			return m_call_context.m_return_value;
		}

	public:
		native_call_context m_call_context;
		static inline std::array<rage::scrNativeHandler, sizeof(g_crossmap) / sizeof(rage::scrNativePair)> m_handler_cache;
		bool m_handlers_cached = false;
	};

	inline native_invoker g_native_invoker;
}
