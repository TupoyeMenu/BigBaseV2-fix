/**
 * @file backend.cpp
 * @brief Main looped threads.
 */

#include "backend.hpp"

#include "looped/looped.hpp"
#include "script.hpp"
#include "script_patches.hpp"

namespace big
{
	void backend::loop()
	{
		register_script_patches();

		while (g_running)
		{
			looped::system_self_globals();

			script::get_current()->yield();
		}
	}
}
