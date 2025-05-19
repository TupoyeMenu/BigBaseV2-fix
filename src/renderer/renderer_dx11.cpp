/**
 * @file renderer.cpp
 * @brief General rendering and ImGui initialization.
 */

#ifdef ENABLE_GUI

#include "renderer_dx11.hpp"

#include "file_manager.hpp"
#include "pointers.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include <imgui.h>
#include <imgui_internal.h>


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer_dx11::renderer_dx11() :
	    m_dxgi_swapchain(*g_pointers->m_swapchain)
	{
		if (m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_d3d_device)) < 0)
		{
			throw std::runtime_error("Failed to get D3D device.");
		}
		m_d3d_device->GetImmediateContext(&m_d3d_device_context);

		init_imgui_config();

		ImGui_ImplDX11_Init(m_d3d_device, m_d3d_device_context);
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		init_imgui_fonts();

		g_renderer = this;
	}

	renderer_dx11::~renderer_dx11()
	{
		g_renderer = nullptr;

		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
	}

	bool renderer_dx11::add_dx_callback(dx_callback callback, std::uint32_t priority)
	{
		if (!m_dx_callbacks.insert({priority, callback}).second)
		{
			LOG(WARNING) << "Duplicate priority given on DX Callback!";

			return false;
		}
		return true;
	}

	void renderer_dx11::add_wndproc_callback(wndproc_callback callback)
	{
		m_wndproc_callbacks.emplace_back(callback);
	}

	void renderer_dx11::on_present()
	{
		new_frame();
		for (const auto& cb : m_dx_callbacks)
			cb.second();
		end_frame();
	}

	void renderer_dx11::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer_dx11::post_reset()
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer_dx11::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		for (const auto& cb : m_wndproc_callbacks)
			cb(hwnd, msg, wparam, lparam);

		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}

	void renderer_dx11::new_frame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void renderer_dx11::end_frame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}

#endif
