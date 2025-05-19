#ifdef ENABLE_GUI

#include "hooking/hooking.hpp"
#include "renderer/renderer.hpp"

namespace big
{
	HRESULT hooks::swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags)
	{
		if (g_running && !g_renderer->is_resizing())
		{
			g_renderer->on_present();
		}

		return g_hooking->get_original<hooks::swapchain_present>()(this_, sync_interval, flags);
	}
}

#endif
