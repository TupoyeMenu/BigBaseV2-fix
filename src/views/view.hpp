/**
 * @file view.hpp
 * @brief GUI window function declarations.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef ENABLE_GUI

#include <imgui.h>

namespace big
{
	class view
	{
	public:
		static void root();
		static void menu_bar();
		static void main_view();

		static void debug_tabs();
		static void debug_demo();
		static void debug_threads();
	};
}

#endif
