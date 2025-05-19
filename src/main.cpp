/**
 * @file main.cpp
 * @brief File with the DllMain function, used for initialization.
 */

#include "backend/backend.hpp"
#include "byte_patch_manager.hpp"
#include "common.hpp"
#include "fiber_pool.hpp"
#include "file_manager.hpp"
#include "gta/joaat.hpp"
#include "hooking/hooking.hpp"
#include "native_hooks/native_hooks.hpp"
#include "pointers.hpp"
#include "renderer/renderer_dx11.hpp"
#include "renderer/renderer_dx12.hpp"
#include "script_mgr.hpp"
#include "services/script_patcher/script_patcher_service.hpp"
#include "thread_pool.hpp"
#include "util/is_enhanced.hpp"


#ifdef ENABLE_EXCEPTION_HANDLER
	#include "logger/exception_handler.hpp"
#endif
#ifdef ENABLE_GUI
	#include "gui.hpp"
	#include "renderer/renderer.hpp"
#endif

#include <rage/gameSkeleton.hpp>

namespace big
{
	bool disable_anticheat_skeleton()
	{
		bool patched = false;
		for (rage::game_skeleton_update_mode* mode = g_pointers->m_game_skeleton->m_update_modes; mode; mode = mode->m_next)
		{
			for (rage::game_skeleton_update_base* update_node = mode->m_head; update_node; update_node = update_node->m_next)
			{
				if (update_node->m_hash != RAGE_JOAAT("Common Main"))
					continue;
				rage::game_skeleton_update_group* group = reinterpret_cast<rage::game_skeleton_update_group*>(update_node);
				for (rage::game_skeleton_update_base* group_child_node = group->m_head; group_child_node;
				    group_child_node                                   = group_child_node->m_next)
				{
					// TamperActions is a leftover from the old AC, but still useful to block anyway
					if (group_child_node->m_hash != 0xA0F39FB6 && group_child_node->m_hash != RAGE_JOAAT("TamperActions"))
						continue;
					patched = true;
					//LOG(INFO) << "Patching problematic skeleton update";
					reinterpret_cast<rage::game_skeleton_update_element*>(group_child_node)->m_function = g_pointers->m_nullsub;
				}
				break;
			}
		}

		for (rage::skeleton_data& i : g_pointers->m_game_skeleton->m_sys_data)
		{
			if (i.m_hash != 0xA0F39FB6 && i.m_hash != RAGE_JOAAT("TamperActions"))
				continue;
			i.m_init_func     = reinterpret_cast<uint64_t>(g_pointers->m_nullsub);
			i.m_shutdown_func = reinterpret_cast<uint64_t>(g_pointers->m_nullsub);
		}
		return patched;
	}
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_is_enhanced = big::is_enhanced();

		g_hmodule     = hmod;
		g_main_thread = CreateThread(
		    nullptr,
		    0,
		    [](PVOID) -> DWORD {
#ifdef ENABLE_EXCEPTION_HANDLER
			    auto handler = exception_handler();
#endif
			    std::srand(std::chrono::system_clock::now().time_since_epoch().count());

			    LPCSTR lpClassName = g_is_enhanced ? "sgaWindow" : "grcWindow";
			    while (!FindWindow(lpClassName, nullptr))
				    std::this_thread::sleep_for(1s);

			    std::filesystem::path base_dir = std::getenv("appdata");
			    base_dir /= PROJECT_NAME;
			    g_file_manager.init(base_dir);
			    g_log.initialize(PROJECT_NAME,
			        g_file_manager.get_project_file("./cout.log")
#ifndef ENABLE_GUI
			            ,
			        false // Disable log window when GUI is disabled.
#endif
			    );
			    try
			    {
				    LOG(INFO) << R"kek(
 ______  _       ______                        ______  
(____  \(_)     (____  \                      (_____ \
 ____)  )_  ____ ____)  ) ____  ___  ____ _   _ ____) )
|  __  (| |/ _  |  __  ( / _  |/___)/ _  ) | | /_____/ 
| |__)  ) ( ( | | |__)  | ( | |___ ( (/ / \ V /_______ 
|______/|_|\_|| |______/ \_||_(___/ \____) \_/(_______)
          (_____|)kek";

				    auto thread_pool_instance = std::make_unique<thread_pool>();
				    LOG(INFO) << "Thread pool initialized.";

				    g.init(g_file_manager.get_project_file("./settings.json"));
				    LOG(INFO) << "Settings Loaded.";

				    auto pointers_instance = std::make_unique<pointers>();
				    LOG(INFO) << "Pointers initialized.";

				    if (!g_is_enhanced)
				    {
					    while (!disable_anticheat_skeleton())
					    {
						    LOG(WARNING) << "Failed patching anticheat gameskeleton (injected too early?). Waiting 500ms and trying again";
						    std::this_thread::sleep_for(500ms);
					    }
					    LOG(INFO) << "Disabled anticheat gameskeleton.";
				    }

				    auto byte_patch_manager_instance = std::make_unique<byte_patch_manager>();
				    LOG(INFO) << "Byte Patch Manager initialized.";

#ifdef ENABLE_GUI
				    std::unique_ptr<renderer_dx11> dx11_renderer_instance;
				    std::unique_ptr<renderer_dx12> dx12_renderer_instance;
				    if (g_is_enhanced)
				    {
					    while (!*g_pointers->m_resolution_x)
					    {
						    std::this_thread::sleep_for(1s);
					    }
					    dx12_renderer_instance = std::make_unique<renderer_dx12>();
				    }
				    else
				    {
					    dx11_renderer_instance = std::make_unique<renderer_dx11>();
				    }

				    LOG(INFO) << "Renderer initialized.";
				    auto gui_instance = std::make_unique<gui>();
#endif

				    auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
				    LOG(INFO) << "Fiber pool initialized.";

				    auto hooking_instance = std::make_unique<hooking>();
				    LOG(INFO) << "Hooking initialized.";

				    auto script_patcher_service_instance = std::make_unique<script_patcher_service>();
				    LOG(INFO) << "Script Patcher initialized.";

				    g_script_mgr.add_script(std::make_unique<script>(&backend::loop));
#ifdef ENABLE_GUI
				    g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));
#endif
				    LOG(INFO) << "Scripts registered.";

				    g_hooking->enable();
				    LOG(INFO) << "Hooking enabled.";

				    auto native_hooks_instance = std::make_unique<native_hooks>();
				    LOG(INFO) << "Dynamic native hooker initialized.";

				    while (g_running)
					    std::this_thread::sleep_for(500ms);

				    g_hooking->disable();
				    LOG(INFO) << "Hooking disabled.";

				    g_script_mgr.remove_all_scripts();
				    LOG(INFO) << "Scripts unregistered.";

				    // cleans up the thread responsible for saving settings
				    g.destroy();

				    // Make sure that all threads created don't have any blocking loops
				    // otherwise make sure that they have stopped executing
				    thread_pool_instance->destroy();
				    LOG(INFO) << "Destroyed thread pool.";

				    script_patcher_service_instance.reset();
				    LOG(INFO) << "Script Patcher Service reset.";

				    hooking_instance.reset();
				    LOG(INFO) << "Hooking uninitialized.";

				    native_hooks_instance.reset();
				    LOG(INFO) << "Dynamic native hooker uninitialized.";

				    fiber_pool_instance.reset();
				    LOG(INFO) << "Fiber pool uninitialized.";

#ifdef ENABLE_GUI
				    if (g_is_enhanced)
					    dx12_renderer_instance.reset();
				    else
					    dx11_renderer_instance.reset();
				    LOG(INFO) << "Renderer uninitialized.";
#endif

				    byte_patch_manager_instance.reset();
				    LOG(INFO) << "Byte Patch Manager uninitialized.";

				    pointers_instance.reset();
				    LOG(INFO) << "Pointers uninitialized.";
			    }
			    catch (std::exception const& ex)
			    {
				    LOG(WARNING) << ex.what();
				    MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			    }

			    LOG(INFO) << "Farewell!";
			    g_log.destroy();

			    CloseHandle(g_main_thread);
			    FreeLibraryAndExitThread(g_hmodule, 0);
		    },
		    nullptr,
		    0,
		    &g_main_thread_id);
	}

	return true;
}
