/**
 * @file backend.cpp
 * @brief Main looped threads.
 */

#include "backend.hpp"

#include "looped/looped.hpp"
#include "script.hpp"

namespace big
{
	void backend::loop()
	{
		while (g_running)
		{
			looped::system_self_globals();

			script::get_current()->yield();
		}
	}
}
