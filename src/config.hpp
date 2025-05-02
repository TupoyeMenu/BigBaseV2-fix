#pragma once

enum class eGameBranch
{
	LEGACY,
	ENCHANCED,

	DONTCARE
};

namespace big
{
	struct menu_build
	{
		static const eGameBranch branch;
		static const int version;
	};
}