#pragma once
#include "common.hpp"

namespace big
{
	/**
	 * @brief GUI rendering functions.
	 */
	class gui
	{
	public:
		void dx_init();

		/**
		* @brief Used to draw the GUI.
		*/
		void dx_on_tick();

		/**
		* @brief Called before big::gui::script_on_tick() loop starts.
		* Can be used to initialize native based GUI.
		*/
		void script_init();
		/**
		* @brief Disables controll when gui is open.
		*/
		void script_on_tick();
		static void script_func();

	public:
		bool m_opened{};
	};

	inline gui g_gui;
}
