/**
 * @file main.cpp
 * @brief File with the DllMain function, used for initialization.
 */

#include "backend/backend.hpp"
#include "common.hpp"
#include "fiber_pool.hpp"
#include "file_manager.hpp"
#include "gui.hpp"
#include "hooking.hpp"
#include "logger.hpp"
#include "native_hooks/native_hooks.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"
#include "services/script_patcher/script_patcher_service.hpp"
#include "thread_pool.hpp"


BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_hmodule     = hmod;
		g_main_thread = CreateThread(
		    nullptr,
		    0,
		    [](PVOID) -> DWORD {
			    while (!FindWindow("grcWindow", "Grand Theft Auto V"))
				    std::this_thread::sleep_for(1s);

			    std::filesystem::path base_dir = std::getenv("appdata");
			    base_dir /= "BigBaseV2";
			    g_file_manager.init(base_dir);
			    auto logger_instance = std::make_unique<logger>("BigBaseV2", g_file_manager.get_project_file("./cout.log"));
			    try
			    {
				    LOG(RAW_GREEN_TO_CONSOLE) << R"kek(
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

				    auto renderer_instance = std::make_unique<renderer>();
				    LOG(INFO) << "Renderer initialized.";
				    auto gui_instance = std::make_unique<gui>();

				    auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
				    LOG(INFO) << "Fiber pool initialized.";

				    auto hooking_instance = std::make_unique<hooking>();
				    LOG(INFO) << "Hooking initialized.";

				    auto script_patcher_service_instance = std::make_unique<script_patcher_service>();
				    LOG(INFO) << "Script Patcher initialized.";

				    g_script_mgr.add_script(std::make_unique<script>(&backend::loop));
				    g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));
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

				    renderer_instance.reset();
				    LOG(INFO) << "Renderer uninitialized.";

				    pointers_instance.reset();
				    LOG(INFO) << "Pointers uninitialized.";
			    }
			    catch (std::exception const& ex)
			    {
				    LOG(WARNING) << ex.what();
				    MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			    }

			    LOG(INFO) << "Farewell!";
			    logger_instance.reset();

			    CloseHandle(g_main_thread);
			    FreeLibraryAndExitThread(g_hmodule, 0);
		    },
		    nullptr,
		    0,
		    &g_main_thread_id);
	}

	return true;
}
