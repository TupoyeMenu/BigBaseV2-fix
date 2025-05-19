#pragma once

#ifdef ENABLE_GUI

	#include "renderer.hpp"

namespace big
{
	class renderer_dx11 final : public renderer
	{
	public:
		explicit renderer_dx11();
		~renderer_dx11();

		/**
		 * @brief Add a callback function to draw your ImGui content in
		 * 
		 * @param callback Function
		 * @param priority The higher the priority the value the later it gets drawn on top
		 * @return true 
		 * @return false 
		 */
		virtual bool add_dx_callback(dx_callback callback, std::uint32_t priority);
		/**
		 * @brief Add a callback function on wndproc
		 * 
		 * @param callback Function
		 */
		virtual void add_wndproc_callback(wndproc_callback callback);

		virtual void on_present();

		virtual void pre_reset();
		virtual void post_reset();

		virtual void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		virtual ExampleDescriptorHeapAllocator* dx12_get_heap_allocator()
		{
			return nullptr;
		}
		virtual bool is_resizing()
		{
			return false;
		}

	private:
		static void new_frame();
		static void end_frame();

	private:
		IDXGISwapChain* m_dxgi_swapchain;
		ID3D11Device* m_d3d_device;
		ID3D11DeviceContext* m_d3d_device_context;

		std::map<std::uint32_t, dx_callback> m_dx_callbacks;
		std::vector<wndproc_callback> m_wndproc_callbacks;
		bool m_restoreState = false;
	};
}

#endif
