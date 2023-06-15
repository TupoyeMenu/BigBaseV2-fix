#pragma once
#include "file_manager.hpp"

#include <bitset>
#include <cstdint>

class CNetGamePlayer;

namespace big
{
	class menu_settings
	{
	public:
		void destroy();
		void init(const file& save_file);

		void attempt_save();
		bool load();

	private:
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();
		bool write_default_config();

	private:
		bool m_running;

		std::unique_ptr<file> m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:

		struct demo
		{
			bool demo_bool = false;
			int demo_int = 1;
			double demo_double = 1.0;
			int demo_combo = 0;
			int64_t demo_bitset = 62678480396171113;
			NLOHMANN_DEFINE_TYPE_INTRUSIVE(demo, demo_bool, demo_int, demo_double, demo_combo, demo_bitset)
		} demo;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, demo)
	};

	inline auto g = menu_settings();
}
