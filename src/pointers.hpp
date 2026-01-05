#pragma once
#include "function_types.hpp"
#include "gta/enums.hpp"
#include "gta/fwddec.hpp"
#include "memory/byte_patch.hpp"
#include "script/scrThread.hpp"

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
#ifdef ENABLE_GUI
		HWND m_hwnd{};
#endif

		eGameState* m_game_state{};
		bool* m_is_session_started{};

		PVOID m_game_skeleton_update;

		CPedFactory** m_ped_factory{};
		CNetworkPlayerMgr** m_network_player_mgr{};

		rage::scrNativeRegistrationTable* m_native_registration_table{};

		rage::atArray<rage::scrThread*>* m_script_threads{};
		rage::scrProgramTable* m_script_program_table{};
		functions::run_script_threads_t m_run_script_threads{};
		std::int64_t** m_script_globals{};
		PVOID m_init_native_tables{};
		functions::script_vm m_script_vm{};

		CGameScriptHandlerMgr** m_script_handler_mgr{};

		IDXGISwapChain** m_swapchain{};
		ID3D12CommandQueue** m_command_queue;

		uint32_t* m_resolution_x;
		uint32_t* m_resolution_y;

		memory::byte_patch* m_model_spawn_bypass;

		functions::ptr_to_handle m_ptr_to_handle{};
		functions::handle_to_ptr m_handle_to_ptr{};

		PVOID m_queue_dependency;
		PVOID m_sig_scan_memory;

		rage::Obf32** m_anticheat_initialized_hash;
		PVOID m_get_anticheat_initialized_hash;
		PVOID m_get_anticheat_initialized_hash_2;
		CAnticheatContext** m_anticheat_context;
	};

	inline pointers* g_pointers{};
}
