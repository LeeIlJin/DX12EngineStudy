#include "pch.h"
#include "imd3d.h"
#include "imwin32.h"
#include <DirectXMath.h>

#include <stdexcept>



#if defined(_DEBUG)

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")


// Assign a name to the object to aid with debugging.
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
	
}

#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

// ComPtr
using Microsoft::WRL::ComPtr;

/*	========================================
*	ERROR 발생할때 쓰로우 하고 출력하기 위해 */
std::string HrToString(HRESULT hr)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}
//	========================================


namespace imd3d
{
	struct FrameContext
	{
		ComPtr<ID3D12CommandAllocator>	CommandAllocator;
		UINT64							FenceValue = 0;
	};

	class DirectX12 : public IDirectX12
	{
	public:
		DirectX12(HWND window_handle, float r, float g, float b);
		virtual ~DirectX12();

	public:
		virtual void WaitForLastSubmittedFrame();

		virtual void FrameStart();
		virtual void Render(void(exe(void)));
		virtual void Resize(UINT width, UINT height);
		virtual void Destroy();

	private:
		inline void Initialize();
		inline void CreateRenderTarget();

		inline void CleanDevice();
		inline void CleanRenderTarget();

		inline FrameContext* WaitForNextFrameResources();

		// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
		// If no such adapter can be found, *ppAdapter will be set to nullptr.
		_Use_decl_annotations_ inline void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

	private:
		/* Pipeline */
		ComPtr<ID3D12Device> m_device;
		ComPtr<IDXGISwapChain3> m_swap_chain;
		HANDLE m_swap_chain_waitable_object;

		/* Descriptor Heaps */
		ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
		ComPtr<ID3D12DescriptorHeap> m_srv_heap;
		UINT m_rtv_descriptor_size;

		ComPtr<ID3D12CommandQueue> m_command_queue;
		ComPtr<ID3D12GraphicsCommandList> m_command_list;

		//ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		//ComPtr<ID3D12DescriptorHeap> m_cbvSrvHeap;
		//ComPtr<ID3D12DescriptorHeap> m_samplerHeap;

		/* Resources */
		ComPtr<ID3D12Resource> m_render_target_resources[FrameCount];
		D3D12_CPU_DESCRIPTOR_HANDLE m_render_target_descriptors[FrameCount];

		/* Assets */
		//ComPtr<ID3D12RootSignature> m_root_signature;
		//ComPtr<ID3D12PipelineState> m_pipeline_state;

		/* For Shync */
		FrameContext m_frame_contexts[FrameCount];
		UINT m_frame_index;
		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fence_event;
		UINT64 m_fence_last_signaled_value;
		
		/* Window */
		HWND m_hwnd;
		int m_width;
		int m_height;

		/* 3D Render */
		//D3D12_VIEWPORT m_viewport;
		//D3D12_RECT m_scissorRect;

		/* Render TEST */
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		//ComPtr<ID3D12Resource> m_vertexBuffer;
		//D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		const float m_clear_color[4];
	};

	DirectX12::DirectX12(HWND window_handle, float r, float g, float b) : m_clear_color{ r,g,b,1.0f }, m_hwnd(window_handle)
	{
		imwin32::get_size(m_hwnd, &m_width, &m_height);

		this->Initialize();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(window_handle);
		ImGui_ImplDX12_Init(m_device.Get(), FrameCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, m_srv_heap.Get(),
			m_srv_heap->GetCPUDescriptorHandleForHeapStart(),
			m_srv_heap->GetGPUDescriptorHandleForHeapStart());
	}

	DirectX12::~DirectX12()
	{
		
	}

	/* PUBLIC ============================================================================== */

	void DirectX12::WaitForLastSubmittedFrame()
	{
		FrameContext* frameCtx = &m_frame_contexts[m_frame_index % FrameCount];

		UINT64 fenceValue = frameCtx->FenceValue;
		if (fenceValue == 0)
			return; // No fence was signaled

		frameCtx->FenceValue = 0;
		if (m_fence->GetCompletedValue() >= fenceValue)
			return;

		m_fence->SetEventOnCompletion(fenceValue, m_fence_event);
		WaitForSingleObject(m_fence_event, INFINITE);
	}

	void DirectX12::FrameStart()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

	}

	void DirectX12::Render(void(exe(void)))
	{
		ImGui::Render();

		FrameContext* frameCtx = WaitForNextFrameResources();
		UINT backBufferIdx = m_swap_chain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_render_target_resources[backBufferIdx].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_command_list->Reset(frameCtx->CommandAllocator.Get(), NULL);
		m_command_list->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		const float clear_color_with_alpha[4] = { m_clear_color[0] * m_clear_color[3], m_clear_color[1] * m_clear_color[3], m_clear_color[2] * m_clear_color[3], m_clear_color[3] };
		m_command_list->ClearRenderTargetView(m_render_target_descriptors[backBufferIdx], clear_color_with_alpha, 0, NULL);
		m_command_list->OMSetRenderTargets(1, &m_render_target_descriptors[backBufferIdx], FALSE, NULL);
		m_command_list->SetDescriptorHeaps(1, &m_srv_heap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_command_list.Get());
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_command_list->ResourceBarrier(1, &barrier);
		m_command_list->Close();

		ID3D12CommandList* temp_command_list = m_command_list.Get();
		m_command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&temp_command_list);

		//m_swap_chain->Present(1, 0); // Present with vsync
		m_swap_chain->Present(0, 0); // Present without vsync

		UINT64 fenceValue = m_fence_last_signaled_value + 1;
		m_command_queue->Signal(m_fence.Get(), fenceValue);
		m_fence_last_signaled_value = fenceValue;
		frameCtx->FenceValue = fenceValue;
	}

	void DirectX12::Resize(UINT width, UINT height)
	{
		if (m_device == NULL)
			return;

		m_width = static_cast<int>(width);
		m_height = static_cast<int>(height);

		WaitForLastSubmittedFrame();
		CleanRenderTarget();

		ThrowIfFailed(m_swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT));

		CreateRenderTarget();
	}

	void DirectX12::Destroy()
	{
		WaitForLastSubmittedFrame();

		// Cleanup
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanDevice();
	}




	/* PRIVATE ============================================================================= */

	inline void DirectX12::Initialize()
	{
		UINT dxgi_factory_flags = 0;

		//	디버그 일경우 
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> dx12_debug = NULL;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12_debug))))
			dx12_debug->EnableDebugLayer();
#endif
		/* FACTORY */
		ComPtr<IDXGIFactory4> factory;
		ThrowIfFailed(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory)));

		/* DEVICE */
		//IDXGIAdapter1* hardware_adapter;
		//GetHardwareAdapter(factory.Get(), &hardware_adapter, true);
		ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

#if defined(_DEBUG)
		if (dx12_debug != NULL)
		{
			ComPtr<ID3D12InfoQueue> info_queue = NULL;
			m_device->QueryInterface(IID_PPV_ARGS(&info_queue));
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			info_queue->Release();
			dx12_debug->Release();
		}
#endif

		/* RTV ( RENDER TARGET VIEW )*/
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = FrameCount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			//ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtv_heap)));

			if (m_device == NULL)
				int a = 10;

			HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtv_heap));
			if (FAILED(hr))
			{
				return;
			}

			m_rtv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_rtv_heap->GetCPUDescriptorHandleForHeapStart();
			for (UINT i = 0; i < FrameCount; i++)
			{
				m_render_target_descriptors[i] = rtv_handle;
				rtv_handle.ptr += m_rtv_descriptor_size;
			}
		}

		/* SRV ( SHADER RESOURCE VIEW ) */
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_srv_heap)));
		}

		/* COMMAND QUEUE */
		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.NodeMask = 1;
			ThrowIfFailed(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_command_queue)));
		}

		/* COMMAND ALLOCATOR */
		{
			for (UINT i = 0; i < FrameCount; i++)
				ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frame_contexts[i].CommandAllocator)));


			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frame_contexts[0].CommandAllocator.Get(), NULL, IID_PPV_ARGS(&m_command_list)));
			ThrowIfFailed(m_command_list->Close());
		}

		/* FENCE */
		{
			ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fence_last_signaled_value = 0;
			m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fence_event == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
		}

		/* SWAP CHAIN */
		{
			DXGI_SWAP_CHAIN_DESC1 desc;
			{
				ZeroMemory(&desc, sizeof(desc));
				desc.BufferCount = FrameCount;
				desc.Width = 0;
				desc.Height = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				desc.Scaling = DXGI_SCALING_STRETCH;
				desc.Stereo = FALSE;
			}

			ComPtr<IDXGISwapChain1> swap_chain1 = NULL;
			ThrowIfFailed(factory->CreateSwapChainForHwnd(m_command_queue.Get(), m_hwnd, &desc, NULL, NULL, &swap_chain1));
			ThrowIfFailed(swap_chain1->QueryInterface(IID_PPV_ARGS(&m_swap_chain)));
			swap_chain1->Release();
			m_swap_chain->SetMaximumFrameLatency(FrameCount);
			m_swap_chain_waitable_object = m_swap_chain->GetFrameLatencyWaitableObject();
		}

		/* RENDER TARGET RESOURCE ( BUFFER ) */
		CreateRenderTarget();

		factory->Release();
	}

	inline void DirectX12::CreateRenderTarget()
	{
		for (UINT i = 0; i < FrameCount; i++)
		{
			ComPtr<ID3D12Resource> back_buffer = NULL;
			m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffer));
			m_device->CreateRenderTargetView(back_buffer.Get(), NULL, m_render_target_descriptors[i]);
			m_render_target_resources[i].Attach(back_buffer.Detach());
		}
	}

	inline void DirectX12::CleanDevice()
	{
		CleanRenderTarget();

		if (m_swap_chain)
		{
			m_swap_chain->SetFullscreenState(FALSE, NULL);
			m_swap_chain->Release();
			m_swap_chain.Reset();
		}

		if (m_swap_chain_waitable_object)
		{
			CloseHandle(m_swap_chain_waitable_object);
			m_swap_chain_waitable_object = NULL;
		}

		for (UINT i = 0; i < FrameCount; i++)
		{
			if (m_frame_contexts[i].CommandAllocator)
			{
				m_frame_contexts[i].CommandAllocator->Release();
				m_frame_contexts[i].CommandAllocator.Reset();
			}
		}

		if (m_command_queue)
		{
			m_command_queue->Release();
			m_command_queue.Reset();
		}

		if (m_command_list)
		{
			m_command_list->Release();
			m_command_list.Reset();
		}

		if (m_rtv_heap)
		{
			m_rtv_heap->Release();
			m_rtv_heap.Reset();
		}

		if (m_srv_heap)
		{
			m_srv_heap->Release();
			m_srv_heap.Reset();
		}

		if (m_fence)
		{
			m_fence->Release();
			m_fence.Reset();
		}

		if (m_fence_event)
		{
			CloseHandle(m_fence_event);
			m_fence_event = NULL;
		}

		if (m_device)
		{
			m_device->Release();
			m_device.Reset();
		}

#if defined(_DEBUG)
		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			debug->Release();
			debug.Reset();
		}
#endif
	}

	inline void DirectX12::CleanRenderTarget()
	{
		WaitForLastSubmittedFrame();

		for (UINT i = 0; i < FrameCount; i++)
		{
			if (m_render_target_resources[i])
			{ 
				m_render_target_resources[i]->Release();
				m_render_target_resources[i].Reset();
			}
		}
	}

	inline FrameContext* DirectX12::WaitForNextFrameResources()
	{
		UINT nextFrameIndex = m_frame_index + 1;
		m_frame_index = nextFrameIndex;

		HANDLE waitableObjects[] = { m_swap_chain_waitable_object, NULL};
		DWORD numWaitableObjects = 1;

		FrameContext* frameCtx = &m_frame_contexts[nextFrameIndex % FrameCount];
		UINT64 fenceValue = frameCtx->FenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->FenceValue = 0;
			m_fence->SetEventOnCompletion(fenceValue, m_fence_event);
			waitableObjects[1] = m_fence_event;
			numWaitableObjects = 2;
		}

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtx;
	}
	
	_Use_decl_annotations_ inline void DirectX12::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}
	

	/* CREATE */
	bool create(HWND window_handle, IDirectX12** out_ptr, float r, float g, float b)
	{
		if (window_handle == NULL)
			return false;

		*out_ptr = new DirectX12(window_handle, r, g, b);

		return true;
	}
}
