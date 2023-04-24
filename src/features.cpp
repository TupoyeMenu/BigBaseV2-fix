/**
 * @file features.cpp
 * @brief Put looped features here.
 */

#include "features.hpp"

#include "common.hpp"
#include "logger.hpp"
#include "natives.hpp"
#include "script.hpp"

namespace big
{
	/**
	 * @brief This function will run every game tick.
	 */
	void features::run_tick()
	{
	}

	void features::script_func()
	{
		while (true)
		{
			run_tick();
			script::get_current()->yield();
		}
	}
}
