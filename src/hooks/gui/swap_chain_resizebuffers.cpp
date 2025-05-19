#ifdef ENABLE_GUI

#include "hooking/hooking.hpp"
#include "renderer/renderer.hpp"

namespace big
{
	HRESULT hooks::swapchain_resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		if (g_running)
		{
			g_renderer->pre_reset();
			auto result = g_hooking->get_original<hooks::swapchain_resizebuffers>()(this_, buffer_count, width, height, new_format, swapchain_flags);
			g_renderer->post_reset();

			return result;
		}

		return g_hooking->get_original<hooks::swapchain_resizebuffers>()(this_, buffer_count, width, height, new_format, swapchain_flags);
	}
}

#endif