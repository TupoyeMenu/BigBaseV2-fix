/**
 * @file pointers.cpp
 * @brief All the signature scans are here.
 */

#include "pointers.hpp"

#include "common.hpp"
#include "memory/all.hpp"

namespace big
{
	pointers::pointers()
	{
		memory::pattern_batch main_batch;

		main_batch.add("Screen Resolution", "66 0F 6E 0D ? ? ? ? 0F B7 3D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_resolution_x = ptr.sub(4).rip().as<uint32_t*>();
			m_resolution_y = ptr.add(4).rip().as<uint32_t*>();
		});
		main_batch.add("Screen Resolution", "75 39 0F 57 C0 F3 0F 2A 05", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_resolution_x = ptr.add(0x5).add(4).rip().as<uint32_t*>();
			m_resolution_y = ptr.add(0x1E).add(4).rip().as<uint32_t*>();
		});


		main_batch.add("Game state", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_game_state = ptr.add(2).rip().add(1).as<eGameState*>();
		});
		main_batch.add("Game state", "83 3D ? ? ? ? ? 0F 85 ? ? ? ? BA ? 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_game_state = ptr.add(2).rip().add(1).as<eGameState*>();
		});

		main_batch.add("Is session started", "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_is_session_started = ptr.add(3).rip().as<bool*>();
		});
		main_batch.add("Is session started", "0F B6 05 ? ? ? ? 0A 05 ? ? ? ? 75 2A", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_is_session_started = ptr.add(3).rip().as<bool*>();
		});

		main_batch.add("Ped factory", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
		});
		main_batch.add("Ped factory", "C7 40 30 03 00 00 00 48 8B 0D", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_ped_factory = ptr.add(7).add(3).rip().as<CPedFactory**>();
		});

		main_batch.add("Network player manager", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
		});
		main_batch.add("Network player manager", "75 0E 48 8B 05 ? ? ? ? 48 8B 88 F0 00 00 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_network_player_mgr = ptr.add(2).add(3).rip().as<CNetworkPlayerMgr**>();
		});

		main_batch.add("Native handlers", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_init_native_tables        = ptr.sub(37).as<PVOID>();
			m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
		});
		main_batch.add("Native handlers", "EB 2A 0F 1F 40 00 48 8B 54 17 10", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_init_native_tables        = ptr.sub(0x2A).as<PVOID>();
			m_native_registration_table = ptr.sub(0xE).rip().as<rage::scrNativeRegistrationTable*>();
		});

		main_batch.add("Script threads", "45 33 F6 8B E9 85 C9 B8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_threads     = ptr.sub(4).rip().sub(8).as<decltype(m_script_threads)>();
			m_run_script_threads = ptr.sub(0x1F).as<functions::run_script_threads_t>();
		});
		main_batch.add("Script threads", "BE 40 5D C6 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_threads     = ptr.add(0x1B).rip().as<decltype(m_script_threads)>();
			m_run_script_threads = ptr.sub(0xA).as<functions::run_script_threads_t>();
		});

		main_batch.add("Script programs", "48 8B 1D ? ? ? ? 41 83 F8 FF", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_program_table = ptr.add(3).rip().as<decltype(m_script_program_table)>();
		});
		main_batch.add("Script programs", "89 46 38 48 8B 0D ? ? ? ? 0F", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_program_table = ptr.add(0x16).rip().as<decltype(m_script_program_table)>();
		});

		main_batch.add("Script globals", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
		});
		main_batch.add("Script globals", "48 8B 8E B8 00 00 00 48 8D 15 ? ? ? ? 49 89 D8", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_globals = ptr.add(10).rip().as<std::int64_t**>();
		});

		main_batch.add("Script VM", "E8 ? ? ? ? 48 85 FF 48 89 1D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_vm = ptr.add(1).rip().as<functions::script_vm>();
		});
		main_batch.add("Script VM", "49 63 41 1C", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_vm = ptr.sub(0x24).as<functions::script_vm>();
		});

		main_batch.add("Swapchain", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
		});
		main_batch.add("Swapchain", "72 C7 EB 02 31 C0 8B 0D", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_command_queue = ptr.add(0x1A).add(3).rip().as<ID3D12CommandQueue**>();
			m_swapchain = ptr.add(0x21).add(3).rip().as<IDXGISwapChain**>();
		});

		main_batch.add("Model Spawn Bypass", "48 8B C8 FF 52 30 84 C0 74 05 48", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_model_spawn_bypass = ptr.add(8).as<PVOID>();
		});

		main_batch.add("Ptr To Handle", "48 8B F9 48 83 C1 10 33 DB", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_ptr_to_handle = ptr.sub(0x15).as<decltype(m_ptr_to_handle)>();
		});
		main_batch.add("Handle To Ptr", "83 F9 FF 74 31 4C 8B 0D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_handle_to_ptr = ptr.as<decltype(m_handle_to_ptr)>();
		});
		main_batch.add("HandlesAndPtrs", "0F 1F 84 00 00 00 00 00 89 F8 0F 28 FE 41", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_handle_to_ptr = ptr.add(0x21).add(1).rip().as<decltype(m_handle_to_ptr)>();
			m_ptr_to_handle = ptr.sub(0xB).add(1).rip().as<decltype(m_ptr_to_handle)>();
		});

		main_batch.add("Queue Dependency", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 8B F2 49 8B F8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_queue_dependency = ptr.as<PVOID>();
		});
		main_batch.add("Queue Dependency", "0F 29 46 50 48 8D 05", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_queue_dependency = ptr.add(0x71).add(1).rip().as<PVOID>();
			m_sig_scan_memory = ptr.add(4).add(3).rip().as<PVOID>();
		});

		main_batch.add("Game Skeleton", "48 8D 0D ? ? ? ? BA ? ? ? ? 74 05 BA ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? ? 48 8D 0D ? ? ? ? BA ? ? ? ? 84 DB 75 05 BA ? ? ? ? E8 ? ? ? ? 48 8B CD C6 05 ? ? ? ? ? E8 ? ? ? ? 84", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_game_skeleton = ptr.add(3).rip().as<rage::game_skeleton*>();
		});

		main_batch.add("Nullsub", "90 C3 CC", [this](memory::handle ptr) {
			m_nullsub = ptr.as<void (*)()>();
		});

		main_batch.run(memory::module(nullptr));

		LPCWSTR lpClassName = g_is_enhanced ? L"sgaWindow" : L"grcWindow";
		m_hwnd = FindWindowW(lpClassName, nullptr);
		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}

	pointers::~pointers()
	{
		g_pointers = nullptr;
	}
}
