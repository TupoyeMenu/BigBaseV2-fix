/**
 * @file renderer.cpp
 * @brief General rendering and ImGui initialization.
 */

#include "renderer/renderer.hpp"
#ifdef ENABLE_GUI

	#include "common.hpp"
	#include "file_manager.hpp"
	#include "fonts/fonts.hpp"
	#include "gui.hpp"
	#include "pointers.hpp"
	#include "renderer_dx12.hpp"

	#include <backends/imgui_impl_dx12.h>
	#include <backends/imgui_impl_win32.h>
	#include <imgui.h>
	#include <imgui_internal.h>


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer_dx12::renderer_dx12()
	{
		if (!g_pointers->m_swapchain)
		{
			LOG(WARNING) << "SwapChain pointer is invalid!";

			return;
		}

		if (!g_pointers->m_command_queue)
		{
			LOG(WARNING) << "CommandQueue pointer is invalid!";

			return;
		}

		//This is required. In order to stop ComPtr from releasing the original pointer, we create a new ComPtr with the ptr as the intializer. (The '=' operator uses swap which releases the object passed into it)
		if (m_GameSwapChain = *(IDXGISwapChain1**)g_pointers->m_swapchain; !m_GameSwapChain)
		{
			LOG(WARNING) << "Dereferenced SwapChain pointer is invalid!";

			return;
		}

		if (m_CommandQueue = *g_pointers->m_command_queue; !m_CommandQueue)
		{
			LOG(WARNING) << "Dereferenced CommandQueue pointer is invalid!";

			return;
		}

		m_GameSwapChain->QueryInterface<IDXGISwapChain3>(&m_SwapChain); //We need GetCurrentBackBufferIndex from IDXGISwapChain3

		if (const auto result = m_SwapChain->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&m_Device)); result < 0)
		{
			LOG(WARNING) << "Failed to get D3D Device with result: [" << result << "]";

			return;
		}

		if (const auto result = m_SwapChain->GetDesc(&m_SwapChainDesc); result < 0)
		{
			LOG(WARNING) << "Failed to get SwapChain Description with result: [" << result << "]";

			return;
		}

		if (const auto result = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_Fence); result < 0)
		{
			LOG(WARNING) << "Failed to create Fence with result: [" << result << "]";

			return;
		}

		if (const auto result = m_FenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr); !result)
		{
			LOG(WARNING) << "Failed to create Fence Event!";

			return;
		}

		m_FrameContext.resize(m_SwapChainDesc.BufferCount);

		D3D12_DESCRIPTOR_HEAP_DESC DescriptorDesc{D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_SwapChainDesc.BufferCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE};
		if (const auto result =
		        m_Device->CreateDescriptorHeap(&DescriptorDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_DescriptorHeap);
		    result < 0)
		{
			LOG(WARNING) << "Failed to create Descriptor Heap with result: [" << result << "]";

			return;
		}

		if (const auto result = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		        __uuidof(ID3D12CommandAllocator),
		        (void**)&m_CommandAllocator);
		    result < 0)
		{
			LOG(WARNING) << "Failed to create primary Command Allocator with result: [" << result << "]";

			return;
		}

		m_FrameContext[0].CommandAllocator = m_CommandAllocator; // set initial command allocator

		// create the rest of the allocators
		for (size_t i = 1; i < m_SwapChainDesc.BufferCount; ++i)
		{
			if (const auto result = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			        __uuidof(ID3D12CommandAllocator),
			        (void**)&m_FrameContext[i].CommandAllocator);
			    result < 0)
			{
				LOG(WARNING) << "Failed to create secondary Command Allocator with result: [" << result << "]";

				return;
			}
		}

		if (const auto result = m_Device->CreateCommandList(0,
		        D3D12_COMMAND_LIST_TYPE_DIRECT,
		        m_CommandAllocator,
		        NULL,
		        __uuidof(ID3D12GraphicsCommandList),
		        (void**)&m_CommandList);
		    result < 0)
		{
			LOG(WARNING) << "Failed to create Command List with result: [" << result << "]";

			return;
		}

		if (const auto result = m_CommandList->Close(); result < 0)
		{
			LOG(WARNING) << "Failed to finalize the creation of Command List with result: [" << result << "]";

			return;
		}

		D3D12_DESCRIPTOR_HEAP_DESC DescriptorBackbufferDesc{D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_SwapChainDesc.BufferCount, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1};
		if (const auto result = m_Device->CreateDescriptorHeap(&DescriptorBackbufferDesc,
		        __uuidof(ID3D12DescriptorHeap),
		        (void**)&m_BackbufferDescriptorHeap);
		    result < 0)
		{
			LOG(WARNING) << "Failed to create Backbuffer Descriptor Heap with result: [" << result << "]";

			return;
		}

		g_renderer = this;

		const auto RTVDescriptorSize{m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)};
		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle{m_BackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};
		for (size_t i = 0; i < m_SwapChainDesc.BufferCount; ++i)
		{
			ComPtr<ID3D12Resource> BackBuffer{};
			m_FrameContext[i].Descriptor = RTVHandle;
			m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)BackBuffer.GetAddressOf());
			m_Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, RTVHandle);
			m_FrameContext[i].Resource = BackBuffer.Get();
			RTVHandle.ptr += RTVDescriptorSize;
		}

		m_HeapAllocator.Create(m_Device, m_DescriptorHeap);

		// never returns false, useless to check return
		ImGui::CreateContext(&m_FontAtlas);
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device                  = m_Device;
		init_info.CommandQueue            = m_CommandQueue;
		init_info.NumFramesInFlight       = m_SwapChainDesc.BufferCount;
		init_info.RTVFormat               = DXGI_FORMAT_R8G8B8A8_UNORM;
		init_info.DSVFormat               = DXGI_FORMAT_UNKNOWN;
		// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
		// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
		init_info.SrvDescriptorHeap = m_DescriptorHeap;
		init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
			return g_renderer->dx12_get_heap_allocator()->Alloc(out_cpu_handle, out_gpu_handle);
		};
		init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
			return g_renderer->dx12_get_heap_allocator()->Free(cpu_handle, gpu_handle);
		};
		ImGui_ImplDX12_Init(&init_info);

		ImGui::StyleColorsDark();

		LOG(INFO) << "DirectX 12 renderer has finished initializing.";
	}

	renderer_dx12::~renderer_dx12()
	{
		g_renderer = nullptr;

		ImGui_ImplWin32_Shutdown();
		wait_for_last_frame();
		ImGui_ImplDX12_Shutdown();

		ImGui::DestroyContext();
	}

	void renderer_dx12::wait_for_last_frame()
	{
		FrameContext FrameCtx = m_FrameContext[m_FrameIndex % m_SwapChainDesc.BufferCount];

		UINT64 FenceValue = FrameCtx.FenceValue;

		if (FenceValue == 0)
		{
			return;
		}

		FrameCtx.FenceValue = 0;

		if (m_Fence->GetCompletedValue() >= FenceValue)
		{
			return;
		}

		m_Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);

		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	void renderer_dx12::wait_for_next_frame()
	{
		UINT NextFrameIndex = m_FrameIndex + 1;
		m_FrameIndex        = NextFrameIndex;

		HANDLE WaitableObjects[] = {m_SwapchainWaitableObject, nullptr};
		DWORD NumWaitableObjets  = 1;

		FrameContext FrameCtx = m_FrameContext[NextFrameIndex % m_SwapChainDesc.BufferCount];
		UINT64 FenceValue     = FrameCtx.FenceValue;
		if (FenceValue != 0) // means no fence was signaled
		{
			FrameCtx.FenceValue = 0;
			m_Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);
			WaitableObjects[1] = m_FenceEvent;
			NumWaitableObjets  = 2;
		}

		WaitForMultipleObjects(NumWaitableObjets, WaitableObjects, TRUE, INFINITE);
	}

	bool renderer_dx12::add_dx_callback(dx_callback callback, std::uint32_t priority)
	{
		if (!m_dx_callbacks.insert({priority, callback}).second)
		{
			LOG(WARNING) << "Duplicate priority given on DX Callback!";

			return false;
		}
		return true;
	}

	void renderer_dx12::add_wndproc_callback(wndproc_callback callback)
	{
		m_wndproc_callbacks.emplace_back(callback);
	}

	void renderer_dx12::on_present()
	{
		new_frame();
		for (const auto& cb : m_dx_callbacks)
			cb.second();
		end_frame();
	}

	void renderer_dx12::pre_reset()
	{
		set_resizing(true);

		wait_for_last_frame();

		ImGui_ImplDX12_InvalidateDeviceObjects();

		for (size_t i{}; i != m_SwapChainDesc.BufferCount; ++i)
		{
			REL(m_FrameContext[i].Resource);
		}
	}

	void renderer_dx12::post_reset()
	{
		//Recreate our pointers and ImGui's
		ImGui_ImplDX12_CreateDeviceObjects();
		const auto RTVDescriptorSize{m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)};
		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle{m_BackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};
		for (size_t i{}; i != m_SwapChainDesc.BufferCount; ++i)
		{
			ComPtr<ID3D12Resource> BackBuffer{};
			m_FrameContext[i].Descriptor = RTVHandle;
			m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)BackBuffer.GetAddressOf());
			m_Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, RTVHandle);
			m_FrameContext[i].Resource = BackBuffer.Get();
			RTVHandle.ptr += RTVDescriptorSize;
		}

		set_resizing(false);
	}

	void renderer_dx12::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		for (const auto& cb : m_wndproc_callbacks)
			cb(hwnd, msg, wparam, lparam);

		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}

	void renderer_dx12::new_frame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void renderer_dx12::end_frame()
	{
		wait_for_next_frame();

		FrameContext& CurrentFrameContext{m_FrameContext[m_SwapChain->GetCurrentBackBufferIndex()]};
		CurrentFrameContext.CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER Barrier{D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		    D3D12_RESOURCE_BARRIER_FLAG_NONE,
		    {{CurrentFrameContext.Resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET}}};
		m_CommandList->Reset(CurrentFrameContext.CommandAllocator, nullptr);
		m_CommandList->ResourceBarrier(1, &Barrier);
		m_CommandList->OMSetRenderTargets(1, &CurrentFrameContext.Descriptor, FALSE, nullptr);
		m_CommandList->SetDescriptorHeaps(1, &m_DescriptorHeap);

		ImGui::Render();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList);

		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_CommandList->ResourceBarrier(1, &Barrier);
		m_CommandList->Close();

		ID3D12CommandList* CommandLists[]{m_CommandList};
		m_CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

		UINT64 FenceValue = m_FenceLastSignaledValue + 1;
		m_CommandQueue->Signal(m_Fence, FenceValue);
		m_FenceLastSignaledValue       = FenceValue;
		CurrentFrameContext.FenceValue = FenceValue;
	}
}

#endif
