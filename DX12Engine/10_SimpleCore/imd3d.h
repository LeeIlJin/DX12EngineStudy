#pragma once

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
	static const UINT FrameCount = 2;

	class IDirectX12
	{
	public:
		
		virtual void WaitForPreviousFrame() = 0;
		virtual void PopulateCommandList() = 0;
	};

	IDirectX12* create(HWND window_handle, ComPtr<IDirectX12>& out_ptr);
}