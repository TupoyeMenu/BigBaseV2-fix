#pragma once
#include "file_manager.hpp"

#include <bitset>
#include <cstdint>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

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

		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		struct settings
		{
			struct hotkeys
			{
				int menu_toggle          = VK_INSERT;
				bool editing_menu_toggle = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(hotkeys, menu_toggle)
			} hotkeys{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(settings, hotkeys)
		} settings{};

		struct demo
		{
			bool demo_bool                 = false;
			int demo_int                   = 1;
			double demo_double             = 1.0;
			int demo_combo                 = 0;
			int64_t demo_bitset            = 62678480396171113;
			std::string demo_vehicle_model = "";
			NLOHMANN_DEFINE_TYPE_INTRUSIVE(demo, demo_bool, demo_int, demo_double, demo_combo, demo_bitset, demo_vehicle_model)
		} demo{};

		struct window
		{
			bool main              = true;
			bool demo              = false;
			ImFont* font_title     = nullptr;
			ImFont* font_sub_title = nullptr;
			ImFont* font_small     = nullptr;
			ImFont* font_icon      = nullptr;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(window, main, demo)
		} window{};

		bool in_script_vm = false;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, demo, window, settings)
	};

	inline auto g = menu_settings();
}
