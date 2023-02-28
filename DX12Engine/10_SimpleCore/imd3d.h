#pragma once

/*
檬扁拳 >>
	颇捞橇扼牢
	{
		叼滚弊 拌摸 汲沥
		Device 积己
		Command Queue 积己
		Sawp Chain 积己
		RTV Descriptor Heap 积己
		Command Allocator 积己
	}

	俊悸
	{
		Root Signature 积己
		Shader 哪颇老
		Vertex Input Layout 积己
		Pipeline State 积己
		Command List 积己
		Command List 摧扁
		Vertex Buffer 肺靛 棺 积己
		Vertex Buffer View 积己
		Fence 积己
		Fence Event Handle 积己
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