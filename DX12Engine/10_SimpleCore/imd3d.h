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
	static const UINT FrameCount = 3;

	class IDirectX12
	{
	public:
		virtual void WaitForLastSubmittedFrame() = 0;

		virtual void FrameStart() = 0;
		virtual void Render(void(exe(void))) = 0;
		virtual void Resize(UINT width, UINT height) = 0;

		virtual void Destroy() = 0;
	};

	bool create(HWND window_handle, IDirectX12** out_ptr, float r = 0.0f, float g = 0.2f, float b = 0.4f);
}