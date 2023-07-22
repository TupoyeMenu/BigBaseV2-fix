#pragma once
#include "function_types.hpp"
#include "gta/enums.hpp"
#include "gta/fwddec.hpp"

#include <memory/handle.hpp>
#include <player/CPlayerInfo.hpp>

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
		std::int64_t** m_script_globals{};

		GenericPool** m_ped_pool{};
		GenericPool** m_prop_pool{};
		VehiclePool*** m_vehicle_pool{};
		GenericPool** m_pickup_pool{};
		GenericPool** m_camera_pool{};

		CGameScriptHandlerMgr** m_script_handler_mgr{};

		IDXGISwapChain** m_swapchain{};

		PVOID m_model_spawn_bypass;

		functions::ptr_to_handle m_ptr_to_handle{};
		functions::handle_to_ptr m_handle_to_ptr{};

		PVOID m_queue_dependency;
	};

	inline pointers* g_pointers{};
}
