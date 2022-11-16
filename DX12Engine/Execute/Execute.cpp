#include "Execute.h"

/* 윈도우 이름 정의 */
#define MAIN_WINDOW L"MainWindow"
#define MAIN_WINDOW_CLASS L"MainClass"

/*
https://www.braynzarsoft.net/viewtutorial/q16390-setting-up-directx-12-for-visual-studio-2015
D3D12 세팅은 여기서 보세요.
*/

imwin32::wnd* mw = nullptr;

void Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    imwin32::WNDCLASSDESC class_desc = imwin32::default_class_desc(MAIN_WINDOW_CLASS, hInstance);
    class_desc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXECUTE));
    class_desc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    imwin32::cls::register_(class_desc);

    imwin32::WNDDESC window_desc = imwin32::main_window_desc(MAIN_WINDOW, MAIN_WINDOW_CLASS, hInstance);
    mw = imwin32::wnd::create_(window_desc);
    imwin32::show(mw->handle);
    imwin32::update(mw->handle);

}

void Loop()
{
    int x, y, width, height;

    imwin32::get_transform(mw, &x, &y, &width, &height);
    int s = imwin32::get_show_state(mw);

    imwin32::set_position(mw, 220, 100);
    imwin32::set_size(mw, 500, 500);
    int a = 0;
}

void Release()
{
    imwin32::release_all();
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
