#pragma once

#ifdef ENABLE_GUI

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

		virtual void pre_reset() = 0;
		virtual void post_reset() = 0;

		virtual void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

		// DO NOT CALL THIS OUTSIDE OF DX12 CODE
		virtual ExampleDescriptorHeapAllocator* dx12_get_heap_allocator() = 0;
		virtual bool is_resizing() = 0;
	};

	inline renderer* g_renderer{};
}

#endif
