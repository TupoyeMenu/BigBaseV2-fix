/**
 * @file renderer.cpp
 * @brief General rendering and ImGui initialization.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>. 
 */

#include "renderer.hpp"

#include "common.hpp"
#include "file_manager.hpp"
#include "fonts/fonts.hpp"
#include "gui.hpp"
#include "pointers.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include <imgui.h>
#include <imgui_internal.h>


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer::renderer() :
	    m_dxgi_swapchain(*g_pointers->m_swapchain)
	{
		if (m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_d3d_device)) < 0)
		{
			throw std::runtime_error("Failed to get D3D device.");
		}
		m_d3d_device->GetImmediateContext(&m_d3d_device_context);

		auto file_path = g_file_manager.get_project_file("./imgui.ini").get_path();

		ImGuiContext* ctx = ImGui::CreateContext();

		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename     = path.c_str();

		auto& io = ImGui::GetIO();

		/**
		 * @todo Add a toggle for Keyboard Controls, as they partially broken in Proton GE.
		 */
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui_ImplDX11_Init(m_d3d_device, m_d3d_device_context);
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		folder windows_fonts(std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts");

		file font_file_path = windows_fonts.get_file("./msyh.ttc");
		if (!font_file_path.exists())
			font_file_path = windows_fonts.get_file("./msyh.ttf");
		auto font_file            = std::ifstream(font_file_path.get_path(), std::ios::binary | std::ios::ate);
		const auto font_data_size = static_cast<int>(font_file.tellg());
		const auto font_data      = std::make_unique<std::uint8_t[]>(font_data_size);

		font_file.seekg(0);
		font_file.read(reinterpret_cast<char*>(font_data.get()), font_data_size);
		font_file.close();

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt20px");

			io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik),
			    sizeof(font_rubik),
			    20.f,
			    &fnt_cfg,
			    io.Fonts->GetGlyphRangesDefault());
			fnt_cfg.MergeMode = true;
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 20.f, &fnt_cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 20.f, &fnt_cfg, io.Fonts->GetGlyphRangesCyrillic());
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt28px");

			g.window.font_title = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 28.f, &fnt_cfg);
			fnt_cfg.MergeMode = true;
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 28.f, &fnt_cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 28.f, &fnt_cfg, io.Fonts->GetGlyphRangesCyrillic());
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt24px");

			g.window.font_sub_title = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 24.f, &fnt_cfg);
			fnt_cfg.MergeMode = true;
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 24.f, &fnt_cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 24.f, &fnt_cfg, io.Fonts->GetGlyphRangesCyrillic());
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt18px");

			g.window.font_small = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 18.f, &fnt_cfg);
			fnt_cfg.MergeMode = true;
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 18.f, &fnt_cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			io.Fonts->AddFontFromMemoryTTF(font_data.get(), font_data_size, 18.f, &fnt_cfg, io.Fonts->GetGlyphRangesCyrillic());
			io.Fonts->Build();
		}

		g_renderer = this;
	}

	renderer::~renderer()
	{
		g_renderer = nullptr;

		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
	}

	bool renderer::add_dx_callback(dx_callback callback, std::uint32_t priority)
	{
		if (!m_dx_callbacks.insert({priority, callback}).second)
		{
			LOG(WARNING) << "Duplicate priority given on DX Callback!";

			return false;
		}
		return true;
	}

	void renderer::add_wndproc_callback(wndproc_callback callback)
	{
		m_wndproc_callbacks.emplace_back(callback);
	}

	void renderer::on_present()
	{
		new_frame();
		for (const auto& cb : m_dx_callbacks)
			cb.second();

		for (const auto& cb : m_present_callbacks)
		{
			m_restoreState = m_state_saver->save_current_state(m_d3d_device_context);
			cb(m_dxgi_swapchain);
			if (m_restoreState)
				m_state_saver->restore_saved_state();
		}

		end_frame();
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		for (const auto& cb : m_wndproc_callbacks)
			cb(hwnd, msg, wparam, lparam);

		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}

	void renderer::new_frame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void renderer::end_frame()
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

//====================================================================================================================================================================
//State Saver Class

// Construct
state_saver::state_saver() :
    m_savedState(false),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0),
    m_pContext(NULL),
    m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
    m_pInputLayout(NULL),
    m_pBlendState(NULL),
    m_sampleMask(0xffffffff),
    m_pDepthStencilState(NULL),
    m_stencilRef(0),
    m_pRasterizerState(NULL),
    m_pPSSRV(NULL),
    m_pSamplerState(NULL),
    m_pVS(NULL),
    m_numVSClassInstances(0),
    m_pVSConstantBuffer(NULL),
    m_pGS(NULL),
    m_numGSClassInstances(0),
    m_pGSConstantBuffer(NULL),
    m_pGSSRV(NULL),
    m_pPS(NULL),
    m_numPSClassInstances(0),
    m_pHS(NULL),
    m_numHSClassInstances(0),
    m_pDS(NULL),
    m_numDSClassInstances(0),
    m_pVB(NULL),
    m_vertexStride(0),
    m_vertexOffset(0),
    m_pIndexBuffer(NULL),
    m_indexFormat(DXGI_FORMAT_UNKNOWN),
    m_indexOffset(0)
{
	for (int i = 0; i < 4; ++i)
	{
		m_blendFactor[i] = 0.0f;
	}
	for (int i = 0; i < 256; ++i)
	{
		m_pVSClassInstances[i] = NULL;
		m_pGSClassInstances[i] = NULL;
		m_pPSClassInstances[i] = NULL;
		m_pHSClassInstances[i] = NULL;
		m_pDSClassInstances[i] = NULL;
	}
}

// Destruct
state_saver::~state_saver()
{
	release_saved_state();
}

// Save all states that are changed by the font-wrapper when drawing a string
bool state_saver::save_current_state(ID3D11DeviceContext* pContext)
{
	if (m_savedState)
		release_saved_state();
	if (pContext == NULL)
		return false;

	ID3D11Device* pDevice;
	pContext->GetDevice(&pDevice);
	if (pDevice != NULL)
	{
		m_featureLevel = pDevice->GetFeatureLevel();
		pDevice->Release();
	}

	pContext->AddRef();
	m_pContext = pContext;

	m_pContext->IAGetPrimitiveTopology(&m_primitiveTopology);
	m_pContext->IAGetInputLayout(&m_pInputLayout);

	m_pContext->OMGetBlendState(&m_pBlendState, m_blendFactor, &m_sampleMask);
	m_pContext->OMGetDepthStencilState(&m_pDepthStencilState, &m_stencilRef);

	m_pContext->RSGetState(&m_pRasterizerState);

	m_numVSClassInstances = 256;
	m_pContext->VSGetShader(&m_pVS, m_pVSClassInstances, &m_numVSClassInstances);
	m_pContext->VSGetConstantBuffers(0, 1, &m_pVSConstantBuffer);

	m_numPSClassInstances = 256;
	m_pContext->PSGetShader(&m_pPS, m_pPSClassInstances, &m_numPSClassInstances);
	m_pContext->PSGetShaderResources(0, 1, &m_pPSSRV);
	pContext->PSGetSamplers(0, 1, &m_pSamplerState);

	if (m_featureLevel >= D3D_FEATURE_LEVEL_10_0)
	{
		m_numGSClassInstances = 256;
		m_pContext->GSGetShader(&m_pGS, m_pGSClassInstances, &m_numGSClassInstances);
		m_pContext->GSGetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSGetShaderResources(0, 1, &m_pGSSRV);

		if (m_featureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			m_numHSClassInstances = 256;
			m_pContext->HSGetShader(&m_pHS, m_pHSClassInstances, &m_numHSClassInstances);

			m_numDSClassInstances = 256;
			m_pContext->DSGetShader(&m_pDS, m_pDSClassInstances, &m_numDSClassInstances);
		}
	}

	m_pContext->IAGetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);

	m_pContext->IAGetIndexBuffer(&m_pIndexBuffer, &m_indexFormat, &m_indexOffset);

	m_savedState = true;

	return true;
}


// Restore state
bool state_saver::restore_saved_state()
{
	if (!m_savedState)
		return false;

	m_pContext->IASetPrimitiveTopology(m_primitiveTopology);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->OMSetBlendState(m_pBlendState, m_blendFactor, m_sampleMask);
	m_pContext->OMSetDepthStencilState(m_pDepthStencilState, m_stencilRef);

	m_pContext->RSSetState(m_pRasterizerState);

	m_pContext->VSSetShader(m_pVS, m_pVSClassInstances, m_numVSClassInstances);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pVSConstantBuffer);

	m_pContext->PSSetShader(m_pPS, m_pPSClassInstances, m_numPSClassInstances);
	m_pContext->PSSetShaderResources(0, 1, &m_pPSSRV);
	m_pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	if (m_featureLevel >= D3D_FEATURE_LEVEL_10_0)
	{
		m_pContext->GSSetShader(m_pGS, m_pGSClassInstances, m_numGSClassInstances);
		m_pContext->GSSetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSSetShaderResources(0, 1, &m_pGSSRV);

		if (m_featureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			m_pContext->HSSetShader(m_pHS, m_pHSClassInstances, m_numHSClassInstances);

			m_pContext->DSSetShader(m_pDS, m_pDSClassInstances, m_numDSClassInstances);
		}
	}

	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);

	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_indexFormat, m_indexOffset);

	return true;
}

/* General Misc */
template<typename T>
inline void SafeRelease(T*& p)
{
	if (nullptr != p)
	{
		p->Release();
		p = nullptr;
	}
}
// Release state
void state_saver::release_saved_state()
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pBlendState);
	for (int i = 0; i < 4; ++i)
		m_blendFactor[i] = 0.0f;
	m_sampleMask = 0xffffffff;
	SafeRelease(m_pDepthStencilState);
	m_stencilRef = 0;
	SafeRelease(m_pRasterizerState);
	SafeRelease(m_pPSSRV);
	SafeRelease(m_pSamplerState);
	SafeRelease(m_pVS);
	for (UINT i = 0; i < m_numVSClassInstances; ++i)
		SafeRelease(m_pVSClassInstances[i]);
	m_numVSClassInstances = 0;
	SafeRelease(m_pVSConstantBuffer);
	SafeRelease(m_pGS);
	for (UINT i = 0; i < m_numGSClassInstances; ++i)
		SafeRelease(m_pGSClassInstances[i]);
	m_numGSClassInstances = 0;
	SafeRelease(m_pGSConstantBuffer);
	SafeRelease(m_pGSSRV);
	SafeRelease(m_pPS);
	for (UINT i = 0; i < m_numPSClassInstances; ++i)
		SafeRelease(m_pPSClassInstances[i]);
	m_numPSClassInstances = 0;
	SafeRelease(m_pHS);
	for (UINT i = 0; i < m_numHSClassInstances; ++i)
		SafeRelease(m_pHSClassInstances[i]);
	m_numHSClassInstances = 0;
	SafeRelease(m_pDS);
	for (UINT i = 0; i < m_numDSClassInstances; ++i)
		SafeRelease(m_pDSClassInstances[i]);
	m_numDSClassInstances = 0;
	SafeRelease(m_pVB);
	m_vertexStride = 0;
	m_vertexOffset = 0;
	SafeRelease(m_pIndexBuffer);
	m_indexFormat = DXGI_FORMAT_UNKNOWN;
	m_indexOffset = 0;

	SafeRelease(m_pContext);
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	m_savedState = false;
}
