#include "pch.h"
#include "imwin32.h"

#define CHAR_MAX 100

static std::vector<HWND> window_array;

typedef std::pair<LPCWSTR, HINSTANCE> CLASS_INFO;
static std::vector<CLASS_INFO> class_array;


HWND imwin32::create_default_window(HINSTANCE hInstance, WNDPROC proc, int width, int height, const wchar_t* title, void* attach)
{
	LPCWSTR class_name = L"DX12EngineMainClass";
	class_array.push_back(CLASS_INFO(class_name, hInstance));

	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = proc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = class_name;
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	HWND hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		class_name,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		attach);

	window_array.push_back(hwnd);

	return hwnd;
}

UINT imwin32::regist_window_class(HINSTANCE hInstance, WNDPROC proc, UINT cs_style, const wchar_t* name)
{
	class_array.push_back(CLASS_INFO(name, hInstance));

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = cs_style;
	windowClass.lpfnWndProc = proc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = name;
	RegisterClassEx(&windowClass);

	return class_array.size() - 1;
}

HWND imwin32::create_window(HINSTANCE hInstance, const wchar_t* class_name, int width, int height, DWORD ws_style, const wchar_t* title, int x, int y, void* attach)
{
	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	HWND hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		class_name,
		title,
		ws_style,
		x,
		y,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		attach);

	window_array.push_back(hwnd);

	return hwnd;
}

HWND imwin32::create_window(UINT class_index, int width, int height, DWORD ws_style, const wchar_t* title, int x, int y, void* attach)
{
	if (class_array.size() < class_index)
		return NULL;

	CLASS_INFO info = class_array[class_index];
	return imwin32::create_window(info.second, info.first, width, height, ws_style, title, x, y, attach);
}

void imwin32::unregist_window_class(HINSTANCE hInstance, const wchar_t* class_name)
{
	UnregisterClassW(class_name, hInstance);
}

void imwin32::unregist_window_class(UINT class_index)
{
	if (class_array.size() < class_index)
		return;

	CLASS_INFO info = class_array[class_index];
	UnregisterClassW(info.first, info.second);
}

void imwin32::destroy_window(HWND handle)
{
	DestroyWindow(handle);
}

void imwin32::clean()
{
	for (UINT i = 0; i < window_array.size(); i++)
	{
		HWND handle = window_array[i];
		DestroyWindow(handle);
	}
	window_array.clear();

	for (UINT i = 0; i < class_array.size(); i++)
	{
		CLASS_INFO info = class_array[i];
		UnregisterClassW(info.first, info.second);
	}
	class_array.clear();
}

void imwin32::get_position(HWND h, int* x, int* y)
{
	RECT r;
	GetWindowRect(h, &r);
	*x = r.left;
	*y = r.top;
}

void imwin32::get_size(HWND h, int* width, int* height)
{
	RECT r;
	GetClientRect(h, &r);
	*width = r.right;
	*height = r.bottom;
}

void imwin32::get_title(HWND h, std::wstring* s)
{
	wchar_t wc[CHAR_MAX];
	GetWindowText(h, wc, CHAR_MAX);
	s->clear();
	s->assign(wc);
}

int imwin32::get_show_state(HWND h)
{
	if (!IsWindowVisible(h))
		return SW_HIDE;
	else if (IsIconic(h))
		return SW_MINIMIZE;
	else if (IsZoomed(h))
		return SW_MAXIMIZE;
	else
		return SW_SHOW;
}

void imwin32::set_position(HWND h, const int& x, const int& y)
{
	SetWindowPos(h, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void imwin32::set_size(HWND h, const int& width, const int& height)
{
	RECT wr = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	SetWindowPos(h, 0, 0, 0, wr.right - wr.left, wr.bottom - wr.top, SWP_NOMOVE | SWP_NOZORDER);
}

void imwin32::set_zorder(HWND h, const bool& top_true_or_bottom_false)
{
	SetWindowPos(h, (top_true_or_bottom_false ? HWND_TOP : HWND_BOTTOM), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void imwin32::set_title(HWND h, std::wstring& s)
{
	SetWindowText(h, s.c_str());
}

void imwin32::show(HWND h)
{
	ShowWindow(h, SW_SHOWDEFAULT);
}

void imwin32::hide(HWND h)
{
	ShowWindow(h, SW_HIDE);
}

void imwin32::mini(HWND h)
{
	ShowWindow(h, SW_MINIMIZE);
}

void imwin32::maxi(HWND h)
{
	ShowWindow(h, SW_MAXIMIZE);
}

bool imwin32::is_show(HWND h)
{
	return get_show_state(h) == SW_SHOW;
}

bool imwin32::is_hide(HWND h)
{
	return get_show_state(h) == SW_HIDE;
}

bool imwin32::is_mini(HWND h)
{
	return get_show_state(h) == SW_MINIMIZE;
}

bool imwin32::is_maxi(HWND h)
{
	return get_show_state(h) == SW_MAXIMIZE;
}

void imwin32::update(HWND h)
{
	UpdateWindow(h);
}

void imwin32::enable(HWND h)
{
	EnableWindow(h, TRUE);
}

void imwin32::disable(HWND h)
{
	EnableWindow(h, FALSE);
}

bool imwin32::is_enable(HWND h)
{
	return IsWindowEnabled(h);
}

bool imwin32::is_disable(HWND h)
{
	return !IsWindowEnabled(h);
}

