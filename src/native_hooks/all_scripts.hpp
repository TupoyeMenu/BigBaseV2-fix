/**
 * @file all_scripts.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "hooking.hpp"
#include "native_hooks.hpp"
#include "natives.hpp"
#include "util/scripts.hpp"

namespace big
{
	namespace all_scripts
	{
		inline void SC_TRANSITION_NEWS_SHOW(rage::scrNativeCallContext* src)
		{
			LOG(VERBOSE) << "SC_TRANSITION_NEWS_SHOW";
			src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW(src->get_arg<Any>(0)));
		}

		inline void SC_TRANSITION_NEWS_SHOW_TIMED(rage::scrNativeCallContext* src)
		{
			LOG(VERBOSE) << "SC_TRANSITION_NEWS_SHOW_TIMED";
			src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW_TIMED(src->get_arg<Any>(0), src->get_arg<Any>(0)));
		}

		void RETURN_TRUE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(TRUE);
		}

		void RETURN_FALSE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(FALSE);
		}

		void DO_NOTHING(rage::scrNativeCallContext* src)
		{
		}
	}
}