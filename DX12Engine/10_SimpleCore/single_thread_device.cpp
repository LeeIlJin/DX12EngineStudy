#include "pch.h"
#include "single_thread_device.h"

#include "imwin32.h"

#include "global_functions.hpp"
#include "ImGui/imgui_helper.hpp"


namespace imd3d
{
	SingleThreadDirectX12::SingleThreadDirectX12(HWND window_handle, bool use_msaa, float r, float g, float b)
		: m_clear_color{ r,g,b,1.0f }, m_hwnd(window_handle), m_MSAA_use(use_msaa)
		, m_buffer_index(0), m_fence_values{}
	{
		imwin32::get_size(m_hwnd, &m_width, &m_height);

		this->Initialize();
		this->InitImGui();

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	SingleThreadDirectX12::~SingleThreadDirectX12()
	{

	}

	/* PUBLIC ============================================================================== */

	

	void SingleThreadDirectX12::BeginFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void SingleThreadDirectX12::BeforeRender()
	{
	}

	void SingleThreadDirectX12::EndFrame()
	{
	}


	void SingleThreadDirectX12::Render()
	{
		ImGui::Render();

		// Record all the commands we need to render the scene into the command list.
		PopulateCommandList();

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { m_cmd_list.Get() };
		m_cmd_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present the frame.
		ThrowIfFailed(m_swap_chain->Present(0, 0));
		
		MoveToNextFrame();
	}

	void SingleThreadDirectX12::Resize(UINT width, UINT height)
	{
		if (m_device == nullptr)
			return;

		m_width = static_cast<int>(width);
		m_height = static_cast<int>(height);

		WaitForGPU();
		ClearRTV();
		ClearDSV();

		ThrowIfFailed(m_swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT));

		CreateRenderTargetView();
		CreateDepthStencilView(width, height);

		ResetScreenViewport(static_cast<float>(m_width), static_cast<float>(m_height));
		ResetScissorRect(width, height);
	}

	float SingleThreadDirectX12::AspectRatio() const
	{
		return static_cast<float>(m_width) / static_cast<float>(m_height);
	}

	void SingleThreadDirectX12::Destroy()
	{
		WaitForGPU();

		ClearImGui();
		ClearDevice();
	}




	/* PRIVATE ============================================================================= */

	inline void SingleThreadDirectX12::Initialize()
	{
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		/* FACTORY */
		ComPtr<IDXGIFactory4> factory;
		{
			ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
		}

		/* DEVICE */
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter);
			HRESULT hardware_result = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
			if (FAILED(hardware_result))
			{
				ComPtr<IDXGIAdapter> warpAdapter;
				ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
				ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
			}
		}

		
		/* CHECK MSAA */
		//{
		//	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels = {};
		//	{
		//		ms_quality_levels.Format = ScreenFormat;
		//		ms_quality_levels.SampleCount = MSAA_Level;
		//		ms_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		//	}
		//	ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels)));

		//	m_4xMSAA_quality = ms_quality_levels.NumQualityLevels;
		//	//	MSAA는 항상 0보다 커야 한다. 0이라면 예상치못한 오류로 취급한다.
		//	assert(m_4xMSAA_quality > 0 && "Unexpected MSAA quality level.");
		//}

		/* COMMAND QUEUE */
		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			{
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
			ThrowIfFailed(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmd_queue)));
		}

		
		/* SWAP CHAIN */
		{
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			{
				desc.BufferCount = BufferCount;
				desc.Width = m_width;
				desc.Height = m_height;
				desc.Format = ScreenFormat;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				desc.SampleDesc.Count = 1;
				//desc.SampleDesc.Count = m_MSAA_use ? 4 : 1;
				//desc.SampleDesc.Quality = m_MSAA_use ? (m_4xMSAA_quality - 1) : 0;
			}

			ComPtr<IDXGISwapChain1> temp_swap_chain;
			ThrowIfFailed(factory->CreateSwapChainForHwnd(m_cmd_queue.Get(), m_hwnd, &desc, nullptr, nullptr, &temp_swap_chain));

			/* LOCK FULLSCREEN */
			ThrowIfFailed(factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));

			ThrowIfFailed(temp_swap_chain.As(&m_swap_chain));
			m_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();
		}

		/* DESCRIPTOR HEAP SIZE */
		{
			m_rtv_DescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_dsv_DescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_cbv_srv_DescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		/* RTV DESC HEAP */
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			{
				desc.NumDescriptors = BufferCount;
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			}
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtv_heap)));
		}

		/* DSV DESC HEAP */
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			{
				desc.NumDescriptors = 1;
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			}
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsv_heap)));
		}

		/* COMMAND OBJECTS */
		{
			for (UINT i = 0; i < BufferCount; i++)
			{
				ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmd_allocators[i])));
			}


			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocators[m_buffer_index].Get(), nullptr, IID_PPV_ARGS(&m_cmd_list)));

			// Command lists are created in the recording state, but there is nothing
			// to record yet. The main loop expects it to be closed, so close it now.
			ThrowIfFailed(m_cmd_list->Close());
		}

		

		/* FENCE */
		{
			ThrowIfFailed(m_device->CreateFence(m_fence_values[m_buffer_index], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fence_values[m_buffer_index]++;

			// Create an event handle to use for frame synchronization.
			m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fence_event == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}

			// Wait for the command list to execute; we are reusing the same command 
			// list in our main loop but for now, we just want to wait for setup to 
			// complete before continuing.
			WaitForGPU();
		}

		CreateRenderTargetView();
		CreateDepthStencilView(m_width, m_height);
		ResetScreenViewport(static_cast<float>(m_width), static_cast<float>(m_height));
		ResetScissorRect(static_cast<LONG>(m_width), static_cast<LONG>(m_height));
	}


	inline void SingleThreadDirectX12::CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < BufferCount; i++)
		{
			ThrowIfFailed(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_rtv_buffers[i])));
			m_device->CreateRenderTargetView(m_rtv_buffers[i].Get(), nullptr, rtv_heap_handle);
			rtv_heap_handle.Offset(1, m_rtv_DescriptorSize);
		}
	}

	inline void SingleThreadDirectX12::CreateDepthStencilView(UINT width, UINT height)
	{
		D3D12_RESOURCE_DESC desc;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DepthStencil_Format;
		desc.SampleDesc.Count = m_MSAA_use ? 4 : 1;
		desc.SampleDesc.Quality = m_MSAA_use ? (m_4xMSAA_quality - 1) : 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clear;
		clear.Format = DepthStencil_Format;
		clear.DepthStencil.Depth = 1.0f;
		clear.DepthStencil.Stencil = 0;

		D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(m_device->CreateCommittedResource
		(
			&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, &clear, IID_PPV_ARGS(&m_dsv_buffer)
		));

		m_device->CreateDepthStencilView(m_dsv_buffer.Get(), nullptr, GetDepthStencilView());

		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_dsv_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_cmd_list->ResourceBarrier(1, &barrier);
	}

	inline void SingleThreadDirectX12::ResetScreenViewport(float width, float height)
	{
		m_screen_viewport.TopLeftX = 0.0f;
		m_screen_viewport.TopLeftY = 0.0f;
		m_screen_viewport.Width = width;
		m_screen_viewport.Height = height;
		m_screen_viewport.MinDepth = 0.0f;
		m_screen_viewport.MaxDepth = 1.0f;
	}

	inline void SingleThreadDirectX12::ResetScissorRect(LONG width, LONG height)
	{
		m_scissor_rect = { 0, 0, width, height };
	}

	inline void SingleThreadDirectX12::ClearDevice()
	{
		ClearRTV();
		ClearDSV();

		if (m_swap_chain)
		{
			m_swap_chain->SetFullscreenState(FALSE, NULL);
			m_swap_chain->Release();
			m_swap_chain.Reset();
		}
		for (UINT i = 0; i < BufferCount; i++)
		{
			m_cmd_allocators[i].Reset();
		}

		m_cmd_queue.Reset();
		m_cmd_list.Reset();
		m_rtv_heap.Reset();
		m_dsv_heap.Reset();
		m_fence.Reset();
		m_device.Reset();

		m_device = nullptr;

#if defined(_DEBUG)
		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			debug.Reset();
		}
#endif
	}

	inline void SingleThreadDirectX12::ClearRTV()
	{
		for (UINT i = 0; i < BufferCount; i++)
		{
			m_rtv_buffers[i].Reset();
		}
	}

	inline void SingleThreadDirectX12::ClearDSV()
	{
		m_dsv_buffer.Reset();
	}

	void SingleThreadDirectX12::PopulateCommandList()
	{
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_cmd_allocators[m_buffer_index]->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		//ThrowIfFailed(m_cmd_list->Reset(m_cmd_allocators[m_buffer_index].Get(), m_pipelineState.Get()));

		// Set necessary state.
		//m_cmd_list->SetGraphicsRootSignature(m_rootSignature.Get());
		m_cmd_list->RSSetViewports(1, &m_screen_viewport);
		m_cmd_list->RSSetScissorRects(1, &m_scissor_rect);

		// Indicate that the back buffer will be used as a render target.
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_rtv_buffers[m_buffer_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_cmd_list->ResourceBarrier(1, &barrier);
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), m_buffer_index, m_rtv_DescriptorSize);
		m_cmd_list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		m_cmd_list->ClearRenderTargetView(rtvHandle, m_clear_color, 0, nullptr);
		//m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		//m_commandList->DrawInstanced(3, 1, 0, 0);

		// Indicate that the back buffer will now be used to present.
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_rtv_buffers[m_buffer_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			m_cmd_list->ResourceBarrier(1, &barrier);
		}

		ThrowIfFailed(m_cmd_list->Close());
	}

	void SingleThreadDirectX12::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_fence_values[m_buffer_index];
		ThrowIfFailed(m_cmd_queue->Signal(m_fence.Get(), currentFenceValue));

		// Update the frame index.
		m_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_fence->GetCompletedValue() < m_fence_values[m_buffer_index])
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fence_values[m_buffer_index], m_fence_event));
			WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_fence_values[m_buffer_index] = currentFenceValue + 1;
	}

	void SingleThreadDirectX12::WaitForGPU()
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_cmd_queue->Signal(m_fence.Get(), m_fence_values[m_buffer_index]));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fence_values[m_buffer_index], m_fence_event));
		WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_fence_values[m_buffer_index]++;
	}

	ID3D12Resource* SingleThreadDirectX12::CurrentBackBuffer() const
	{
		return m_rtv_buffers[m_buffer_index].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SingleThreadDirectX12::CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), m_buffer_index, m_rtv_DescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SingleThreadDirectX12::GetDepthStencilView() const
	{
		return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
	}

	void SingleThreadDirectX12::InitImGui()
	{
		/* SRV ( SHADER RESOURCE VIEW ) */
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imgui_srv_heap)));
		}

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
		ImGui_ImplWin32_Init(m_hwnd);
		ImGui_ImplDX12_Init(m_device.Get(), BufferCount,
			ScreenFormat, m_imgui_srv_heap.Get(),
			m_imgui_srv_heap->GetCPUDescriptorHandleForHeapStart(),
			m_imgui_srv_heap->GetGPUDescriptorHandleForHeapStart());
	}

	void SingleThreadDirectX12::ClearImGui()
	{

		// Cleanup
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	



}
