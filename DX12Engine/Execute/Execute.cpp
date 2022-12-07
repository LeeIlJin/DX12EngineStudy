
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        d3d->Render(NULL);
        break;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

void Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    g_main = imwin32::create_main_window(hInstance, WndProc, 1280, 720, L"TestMain");
    imd3d::create(g_main, &d3d);

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
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            Loop();
    }

    Release();
    
    return (int) msg.wParam;
}
