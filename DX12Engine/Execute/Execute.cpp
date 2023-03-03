
#include "Execute.h"

/* 윈도우 이름 정의 */
#define MAIN_WINDOW L"MainWindow"
#define MAIN_WINDOW_CLASS L"MainClass"

/*
https://www.braynzarsoft.net/viewtutorial/q16390-setting-up-directx-12-for-visual-studio-2015
D3D12 세팅은 여기서 보세요.
*/

HWND g_main;
imd3d::IDirectX12* d3d = nullptr;

template<UINT uid>
class Test
{
public:
    static UINT UID() { return uid; }
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w, l))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (d3d != NULL && w != SIZE_MINIMIZED)
        {
            d3d->Resize((UINT)LOWORD(l), (UINT)HIWORD(l));
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SYSCOMMAND:
        if ((w & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_PAINT:
        d3d->Render();
        break;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

void Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    g_main = imwin32::create_default_window(hInstance, WndProc, 1280, 720, L"TestMain");
    imd3d::create_single_thread(&d3d, g_main, true);

    //ShowWindow(g_main, 1);
    imwin32::show(g_main);
    imwin32::update(g_main);

    
}

void Loop()
{
    int x, y, width, height;

    imwin32::get_position(g_main, &x, &y);
    imwin32::get_size(g_main, &width, &height);
    int s = imwin32::get_show_state(g_main);

    //imwin32::set_position(g_main, 220, 100);
    //imwin32::set_size(g_main, 500, 500);
    int a = 0;
}

void Release()
{
    DESTROY(d3d);
    imwin32::clean();
}

/* WIN MAIN */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Initialize(hInstance, lpCmdLine, nCmdShow);
    
    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    
    // 기본 메시지 루프입니다:
    bool done = false;
    while (!done)
    {
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        d3d->BeginFrame();
        Loop();
        d3d->BeforeRender();
        d3d->Render();
        d3d->EndFrame();
    }

    Release();
    
    return (int) msg.wParam;
}
