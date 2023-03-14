#pragma once
#include "asi_loader/pools.hpp"
#include "common.hpp"
#include "function_types.hpp"
#include "gta/enums.hpp"

namespace big
{
	class pointers
	{
	public:
		explicit pointers();
		~pointers();

	public:
		HWND m_hwnd{};

		eGameState* m_game_state{};
		bool* m_is_session_started{};

		CPedFactory** m_ped_factory{};
		CNetworkPlayerMgr** m_network_player_mgr{};

		rage::scrNativeRegistrationTable* m_native_registration_table{};
		functions::get_native_handler_t m_get_native_handler{};
		functions::fix_vectors_t m_fix_vectors{};

		rage::atArray<GtaThread*>* m_script_threads{};
		rage::scrProgramTable* m_script_program_table{};
		functions::run_script_threads_t m_run_script_threads{};
		functions::register_file_t m_register_file{};
		functions::get_script_handle_t m_get_script_handle{};
		std::int64_t** m_script_globals{};

		rage::GenericPool* m_ped_pool{};
		rage::VehiclePool* m_vehicle_pool{};
		rage::GenericPool* m_prop_pool{};
		rage::GenericPool* m_pickup_pool{};
		rage::GenericPool* m_camera_pool{};

		CGameScriptHandlerMgr** m_script_handler_mgr{};

		IDXGISwapChain** m_swapchain{};

		PVOID m_model_spawn_bypass;
		PVOID m_native_return;
	};

	inline pointers* g_pointers{};
}
