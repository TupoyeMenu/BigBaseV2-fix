#pragma once
#include "common.hpp"
#include "gta/fwddec.hpp"
#include "gta/natives.hpp"

namespace big::functions
{
	using run_script_threads_t = bool (*)(std::uint32_t ops_to_execute);
	using get_native_handler_t = rage::scrNativeHandler (*)(rage::scrNativeRegistrationTable*, rage::scrNativeHash);
	using fix_vectors_t        = void (*)(rage::scrNativeCallContext*);
	using register_file_t      = uint32_t* (*)(int*, const char*, bool, const char*, bool);
	using get_script_handle_t  = int64_t (*)(int64_t);
}
