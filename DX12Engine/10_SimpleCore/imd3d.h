#pragma once

#include "pch.h"



/*
�ʱ�ȭ >>
	����������
	{
		����� ���� ����
		Device ����
		Command Queue ����
		Sawp Chain ����
		RTV Descriptor Heap ����
		Command Allocator ����
	}

	����
	{
		Root Signature ����
		Shader ������
		Vertex Input Layout ����
		Pipeline State ����
		Command List ����
		Command List �ݱ�
		Vertex Buffer �ε� �� ����
		Vertex Buffer View ����
		Fence ����
		Fence Event Handle ����
		Wait For GPU!
	}
*/

namespace imd3d
{
	class IDirectX12
	{
	public:
		virtual void BeginFrame() = 0;
		virtual void BeforeRender() = 0;
		virtual void EndFrame() = 0;

		virtual void Render() = 0;
		virtual void Resize(UINT width, UINT height) = 0;

		virtual float AspectRatio() const = 0;

		virtual void Destroy() = 0;
	};


	bool create_single_thread(IDirectX12** out_ptr, HWND window_handle,bool use_msaa, float r = 0.0f, float g = 0.2f, float b = 0.4f);


	class DirectX12Included
	{
	protected:

		// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
		// If no such adapter can be found, *ppAdapter will be set to nullptr.
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);
	};
}