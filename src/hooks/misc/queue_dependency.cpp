#include "hooking/hooking.hpp"
#include "memory/handle.hpp"
#include "memory/module.hpp"
#include "pointers.hpp"
#include "security/ObfVar.hpp"

#include <psapi.h>

namespace big
{
	bool inline is_address_in_game_region(void* address)
	{
		return memory::module(nullptr).contains(memory::handle(address));
	}

	struct ac_verifier
	{
		virtual ~ac_verifier() = 0;
		virtual bool run()     = 0;
		rage::Obf32 m_last_time; // 0x8
		rage::Obf32 m_delay;     // 0x18
	};

	bool is_unwanted_dependency(int64_t cb)
	{
		void* f1 = *reinterpret_cast<void**>(cb + 0x60);
		void* f2 = *reinterpret_cast<void**>(cb + 0x100);
		void* f3 = *reinterpret_cast<void**>(cb + 0x1A0);

		if (!is_address_in_game_region(f1) || !is_address_in_game_region(f2) || !is_address_in_game_region(f3)) [[likely]]
			return false;

		if (*reinterpret_cast<uint8_t*>(f1) != 0xE9) [[likely]]
			return false;

		return true;
	}

	// This is stupid, we already have code for this.
	bool is_unwanted_dependency_enhanced(void* addr)
	{
		if (!addr || !is_address_in_game_region(addr))
			return false;

		auto as_bytes = reinterpret_cast<std::uint8_t*>(addr);

		if (as_bytes[0] != 0xE9)
			return false;

		auto jmp_addr = memory::handle(addr).add(1).rip().as<uint64_t*>();

		if (!is_address_in_game_region(jmp_addr))
			return false;

		return true; // likely, yes
	}

	void hooks::queue_dependency_enhanced(intptr_t a1)
	{
		auto f1 = *reinterpret_cast<void**>(a1 + 0x60);
		auto f2 = *reinterpret_cast<void**>(a1 + 0x100);
		auto f3 = *reinterpret_cast<void**>(a1 + 0x1A0);

		if (!f1)
			return g_hooking->get_original<hooks::queue_dependency_enhanced>()(a1);

		if (f1 == g_pointers->m_sig_scan_memory)
		{
			LOGF(WARNING, "QueueDependency: Blocked the sig scanner dependency from being queued");
			return;
		}

		int arx_score = (int)is_unwanted_dependency_enhanced(f1) + (int)is_unwanted_dependency_enhanced(f2) + (int)is_unwanted_dependency_enhanced(f3);

		if (arx_score >= 2)
		{
			return;
		}

		g_hooking->get_original<hooks::queue_dependency_enhanced>()(a1);
	}


	static bool nullsub()
	{
		return true; // returning false would cause the dependency to requeue
	}

	int hooks::queue_dependency_legacy(void* a1, int a2, int64_t dependency)
	{
		if (is_unwanted_dependency(dependency)) [[unlikely]]
		{
			LOG(INFO) << "Blocking AC Verifier " << HEX_TO_UPPER(*reinterpret_cast<int64_t*>(dependency + 0x60) - reinterpret_cast<int64_t>(GetModuleHandle(0)));
			ac_verifier* verifier                         = reinterpret_cast<ac_verifier*>(dependency - 0x30);
			verifier->m_delay                             = INT_MAX; // makes it so these won't queue in the future
			*reinterpret_cast<void**>(dependency + 0x60)  = (void*)nullsub;
			*reinterpret_cast<void**>(dependency + 0x100) = (void*)nullsub;
			*reinterpret_cast<void**>(dependency + 0x1A0) = (void*)nullsub;
		}

		return g_hooking->get_original<hooks::queue_dependency_legacy>()(a1, a2, dependency);
	}
}