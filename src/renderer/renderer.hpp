#pragma once

#ifdef ENABLE_GUI
	#include "fonts/fonts.hpp"
	#include "imgui_internal.h"

	#include <d3d12.h>
	#include <dxgi1_4.h>
	#include <wrl/client.h>

namespace big
{
	using dx_callback      = std::function<void()>;
	using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

	struct ExampleDescriptorHeapAllocator
	{
		ID3D12DescriptorHeap* Heap          = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
		D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
		UINT HeapHandleIncrement;
		ImVector<int> FreeIndices;

		void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
		{
			IM_ASSERT(Heap == nullptr && FreeIndices.empty());
			Heap                            = heap;
			D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
			HeapType                        = desc.Type;
			HeapStartCpu                    = Heap->GetCPUDescriptorHandleForHeapStart();
			HeapStartGpu                    = Heap->GetGPUDescriptorHandleForHeapStart();
			HeapHandleIncrement             = device->GetDescriptorHandleIncrementSize(HeapType);
			FreeIndices.reserve((int)desc.NumDescriptors);
			for (int n = desc.NumDescriptors; n > 0; n--)
				FreeIndices.push_back(n - 1);
		}
		void Destroy()
		{
			Heap = nullptr;
			FreeIndices.clear();
		}
		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			IM_ASSERT(FreeIndices.Size > 0);
			int idx = FreeIndices.back();
			FreeIndices.pop_back();
			out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
			out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
		}
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
		{
			int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
			int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
			IM_ASSERT(cpu_idx == gpu_idx);
			FreeIndices.push_back(cpu_idx);
		}
	};


	class renderer
	{
	public:
		/**
		 * @brief Add a callback function to draw your ImGui content in
		 * 
		 * @param callback Function
		 * @param priority The higher the priority the value the later it gets drawn on top
		 * @return true 
		 * @return false 
		 */
		virtual bool add_dx_callback(dx_callback callback, std::uint32_t priority) = 0;
		/**
		 * @brief Add a callback function on wndproc
		 * 
		 * @param callback Function
		 */
		virtual void add_wndproc_callback(wndproc_callback callback) = 0;

		virtual void on_present() = 0;

		virtual void pre_reset()  = 0;
		virtual void post_reset() = 0;

		virtual void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

		// DO NOT CALL THIS OUTSIDE OF DX12 CODE
		virtual ExampleDescriptorHeapAllocator* dx12_get_heap_allocator() = 0;
		virtual bool is_resizing()                                        = 0;

		static void init_imgui_config()
		{
			auto file_path = g_file_manager.get_project_file("./imgui.ini").get_path();

			ImGuiContext* ctx = ImGui::CreateContext();

			static std::string path = file_path.make_preferred().string();
			ctx->IO.IniFilename     = path.c_str();

			auto& io = ImGui::GetIO();

			/**
			 * @todo Add a toggle for Keyboard Controls, as they partially broken in Wine.
			 */
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
			// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
			// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows // broken on DX12
		}

		static void init_imgui_fonts()
		{
			auto& io = ImGui::GetIO();
			folder windows_fonts(std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts");

			file font_file_path = windows_fonts.get_file("./msyh.ttc");
			if (!font_file_path.exists())
				font_file_path = windows_fonts.get_file("./msyh.ttf");

			if (!font_file_path.exists())
			{
				LOG(WARNING) << "msyh font not found";
				return;
			}

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
		}
	};

	inline renderer* g_renderer{};
}

#endif
