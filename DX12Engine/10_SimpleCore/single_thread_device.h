#pragma once

#include "imd3d.h"

namespace imd3d
{
	static const UINT BufferCount = 2;
	static const DXGI_FORMAT ScreenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT DepthStencil_Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const UINT MSAA_Level = 4;

	class SingleThreadDirectX12 : public IDirectX12, public DirectX12Included
	{
	public:
		SingleThreadDirectX12(HWND window_handle,bool use_msaa, float r, float g, float b);
		virtual ~SingleThreadDirectX12();

	public:
		

		virtual void BeginFrame();
		virtual void BeforeRender();
		virtual void EndFrame();
		virtual void Render();
		virtual void Resize(UINT width, UINT height);
		virtual float AspectRatio() const;
		virtual void Destroy();

	private:
		void Initialize();
		inline void CreateRenderTargetView();
		inline void CreateDepthStencilView(UINT width, UINT height);

		inline void ResetScreenViewport(float width, float height);
		inline void ResetScissorRect(LONG width, LONG height);

		inline void ClearDevice();
		inline void ClearRTV();
		inline void ClearDSV();

		void PopulateCommandList();
		void MoveToNextFrame();
		void WaitForGPU();


		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;


		/* ImGui */
		void InitImGui();
		void ClearImGui();

	protected:
		ComPtr<ID3D12Device> m_device;
		ComPtr<IDXGISwapChain3> m_swap_chain;
		

		ComPtr<ID3D12CommandQueue> m_cmd_queue;
		ComPtr<ID3D12CommandAllocator> m_cmd_allocators[BufferCount];
		ComPtr<ID3D12GraphicsCommandList> m_cmd_list;

		ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
		ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

		ComPtr<ID3D12Resource> m_rtv_buffers[BufferCount];
		ComPtr<ID3D12Resource> m_dsv_buffer;

		D3D12_VIEWPORT m_screen_viewport;
		RECT m_scissor_rect;

		UINT m_4xMSAA_quality;
		UINT m_rtv_DescriptorSize;
		UINT m_dsv_DescriptorSize;
		UINT m_cbv_srv_DescriptorSize;

		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fence_event;
		INT m_buffer_index;
		UINT64 m_fence_values[BufferCount];

		bool m_MSAA_use;

		/* Window */
		HWND m_hwnd;
		int m_width;
		int m_height;
		float m_clear_color[4];
		
		/* ImGui */
		ComPtr<ID3D12DescriptorHeap> m_imgui_srv_heap;

	};
}
