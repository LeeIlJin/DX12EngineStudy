#include "Execute.h"

/* 윈도우 이름 정의 */
#define MAIN_WINDOW L"MainWindow"
#define MAIN_WINDOW_CLASS L"MainClass"




LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
/* WIN MAIN */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    imwin32::WNDCLASSDESC class_desc = imwin32::default_class_desc(MAIN_WINDOW_CLASS, hInstance, WndProc);
    class_desc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXECUTE));
    class_desc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
    imwin32::cls::register_(class_desc);

    imwin32::WNDDESC window_desc = imwin32::main_window_desc(MAIN_WINDOW, MAIN_WINDOW_CLASS, hInstance);
    imwin32::wnd* main = imwin32::wnd::create_(window_desc);
    imwin32::show(main->handle);
    imwin32::update(main->handle);

    MSG msg;
    
    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }

        // 코드 입력!
    }

    imwin32::release_all();
    return (int) msg.wParam;
}

/* WND PROC*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
