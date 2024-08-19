/**
 * @file hooking.cpp
 * @brief Hooking initialization.
 */

#include "hooking.hpp"

#include "common.hpp"
#include "function_types.hpp"
#include "gta/script_thread.hpp"
#include "gui.hpp"
#include "memory/module.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

#include <MinHook.h>
#include <fibersapi.h>

namespace big
{
	hooking::hooking() :
	    m_swapchain_hook(*g_pointers->m_swapchain, hooks::swapchain_num_funcs)
	{
		m_swapchain_hook.hook(hooks::swapchain_present_index, (void*)&hooks::swapchain_present);
		m_swapchain_hook.hook(hooks::swapchain_resizebuffers_index, (void*)&hooks::swapchain_resizebuffers);

		// The only instances in that vector at this point should only be the "lazy" hooks
		// aka the ones that still don't have their m_target assigned
		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper->m_detour_hook->set_target_and_create_hook(detour_hook_helper->m_on_hooking_available());
		}

		detour_hook_helper::add<hooks::run_script_threads>("Script hook", (void*)g_pointers->m_run_script_threads);
		detour_hook_helper::add<hooks::queue_dependency>("Queue Dependency", (void*)g_pointers->m_queue_dependency);
		detour_hook_helper::add<hooks::init_native_tables>("Init Native Tables", (void*)g_pointers->m_init_native_tables);
		detour_hook_helper::add<hooks::script_vm>("Script VM", (void*)g_pointers->m_script_vm);

		g_hooking = this;
	}

	hooking::~hooking()
	{
		if (m_enabled)
			disable();

		g_hooking = nullptr;
	}

	void hooking::enable()
	{
		m_swapchain_hook.enable();
		m_og_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&hooks::wndproc)));

		for (const auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper->m_detour_hook->enable();
		}

		MH_ApplyQueued();

		m_enabled = true;
	}

	void hooking::disable()
	{
		m_enabled = false;

		for (const auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper->m_detour_hook->disable();
		}

		SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
		m_swapchain_hook.disable();

		MH_ApplyQueued();

		for (const auto& detour_hook_helper : m_detour_hook_helpers)
		{
			delete detour_hook_helper;
		}
		m_detour_hook_helpers.clear();
	}

	hooking::detour_hook_helper::~detour_hook_helper()
	{
		delete m_detour_hook;
	}

	void hooking::detour_hook_helper::enable_hook_if_hooking_is_already_running()
	{
		if (g_hooking && g_hooking->m_enabled)
		{
			if (m_on_hooking_available)
			{
				m_detour_hook->set_target_and_create_hook(m_on_hooking_available());
			}

			m_detour_hook->enable();
			MH_ApplyQueued();
		}
	}

	bool hooks::run_script_threads(std::uint32_t ops_to_execute)
	{
		g_native_invoker.cache_handlers();

		if (g_running)
		{
			g_script_mgr.tick();
		}

		return g_hooking->get_original<run_script_threads>()(ops_to_execute);
	}
}
