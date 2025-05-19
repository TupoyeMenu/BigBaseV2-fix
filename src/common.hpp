#pragma once

// clang-format off

#include <sdkddkver.h>
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <set>
#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <format>
#include <nlohmann/json.hpp>

#include "logger/logger.hpp"

#include "core/settings.hpp"

#include <ped/CPed.hpp>
#include <script/types.hpp>

#include "config.hpp"

// clang-format on

namespace big
{
	using namespace std::chrono_literals;

	template<typename T>
	using comptr = Microsoft::WRL::ComPtr<T>;

	inline HMODULE g_hmodule{};
	inline HANDLE g_main_thread{};
	inline DWORD g_main_thread_id{};
	inline std::atomic_bool g_running{true};
	inline std::atomic_bool g_is_enhanced{false};

	namespace self
	{
		inline Ped ped;
		inline Player id;
		inline Vector3 pos;
		inline Vehicle veh;
	}
}
