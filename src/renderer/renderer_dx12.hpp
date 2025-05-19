#pragma once

#ifdef ENABLE_GUI

	#include "renderer.hpp"

	#define REL(o)       \
		o->Release();    \
		if (o)           \
		{                \
			o = nullptr; \
		}

namespace big
{
	using namespace Microsoft::WRL;

	struct FrameContext
	{
		ID3D12CommandAllocator* CommandAllocator;
		ID3D12Resource* Resource;
		D3D12_CPU_DESCRIPTOR_HANDLE Descriptor;
		UINT64 FenceValue;
	};

	class renderer_dx12 final : public renderer
	{
	public:
		explicit renderer_dx12();
		~renderer_dx12();

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
			return &m_HeapAllocator;
		}

		virtual bool is_resizing()
		{
			return m_Resizing;
		}

	private:
		void new_frame();
		void end_frame();

		void wait_for_last_frame();
		void wait_for_next_frame();

		void set_resizing(const bool& status)
		{
			m_Resizing = status;
		}

	private:
		ExampleDescriptorHeapAllocator m_HeapAllocator;

	private:
		bool m_Resizing;

		//DX12
		std::vector<FrameContext> m_FrameContext;

		DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;
		IDXGISwapChain1* m_GameSwapChain;
		IDXGISwapChain3* m_SwapChain;
		ID3D12Device* m_Device;
		ID3D12CommandQueue* m_CommandQueue;
		ID3D12CommandAllocator* m_CommandAllocator;
		ID3D12GraphicsCommandList* m_CommandList;
		ID3D12DescriptorHeap* m_BackbufferDescriptorHeap;
		ID3D12DescriptorHeap* m_DescriptorHeap;

		ID3D12Fence* m_Fence;
		HANDLE m_FenceEvent{};
		UINT64 m_FenceLastSignaledValue{};
		HANDLE m_SwapchainWaitableObject{};
		UINT64 m_FrameIndex{};

	private:
		std::map<std::uint32_t, dx_callback> m_dx_callbacks;
		std::vector<wndproc_callback> m_wndproc_callbacks;
	};
}

#endif
