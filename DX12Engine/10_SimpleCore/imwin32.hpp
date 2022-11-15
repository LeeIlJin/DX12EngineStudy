#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <unordered_map>
#include <string>
#include <cassert>

#define CHAR_MAX 100

namespace imwin32
{
	typedef WNDCLASSEX WNDCLASSDESC;
	typedef struct tagWNDDESC
	{
	public:
		DWORD		dwExStyle;
		LPCWSTR		lpClassName;
		LPCWSTR		lpWindowName;
		DWORD		dwStyle;
		HWND		hWndParent;
		HMENU		hMenu;
		HINSTANCE	hInstance;
		LPVOID		lpParam;

		int x;
		int y;
		int nWidth;
		int nHeight;
	}WNDDESC;

	/* WNDPROC 예제 입니다
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
	*/


	/* ======================================================================================================== */
	/* Window Class */
	class cls
	{
	private:
		cls(const WNDCLASSDESC& desc)
			:cbSize(desc.cbSize), style(desc.style), lpfnWndProc(desc.lpfnWndProc), cbClsExtra(desc.cbClsExtra), cbWndExtra(desc.cbWndExtra)
			, hInstance(desc.hInstance), hIcon(desc.hIcon), hCursor(desc.hCursor), hbrBackground(desc.hbrBackground), lpszMenuName(desc.lpszMenuName)
			, lpszClassName(desc.lpszClassName), hIconSm(desc.hIconSm)
		{}

		~cls() {}

	public:
		/* tagWNDCLASSEXW */
		const UINT        cbSize;
		/* Win 3.x */
		const UINT        style;
		const WNDPROC     lpfnWndProc;
		const int         cbClsExtra;
		const int         cbWndExtra;
		const HINSTANCE   hInstance;
		const HICON       hIcon;
		const HCURSOR     hCursor;
		const HBRUSH      hbrBackground;
		const LPCWSTR     lpszMenuName;
		const LPCWSTR     lpszClassName;
		/* Win 4.0 */
		const HICON       hIconSm;

	private:
		static std::unordered_map<std::wstring, cls*> map;
	public:
		static bool is_exist_(const wchar_t* name) { return map.count(name) > 0; }
		static cls* get_(const wchar_t* name)
		{
			if (is_exist_(name) == false)
				return nullptr;

			return map[name];
		}
		static cls* register_(const WNDCLASSDESC& desc)
		{
			if (is_exist_(desc.lpszClassName) == true)
				return nullptr;

			WORD wHr = RegisterClassEx(&desc);
			assert(wHr != 0);

			cls* n = new cls(desc);
			map.insert({ desc.lpszClassName, n });
			return n;
		}

		static void release_()
		{
			for (auto it = map.begin(); it != map.end(); it++)
			{
				if (it->second != nullptr)
					delete it->second;
			}
			map.clear();
		}
	};
	std::unordered_map<std::wstring, cls*> cls::map = std::unordered_map<std::wstring, cls*>();


	/* ======================================================================================================== */
	/* Window */
	class wnd
	{
	private:
		wnd(HWND hwnd, const tagWNDDESC& desc)
			:handle(hwnd), dwExStyle(desc.dwExStyle), lpClassName(desc.lpClassName), lpWindowName(desc.lpWindowName)
			, dwStyle(desc.dwStyle), hWndParent(desc.hWndParent), hMenu(desc.hMenu), hInstance(desc.hInstance), lpParam(desc.lpParam)
			, startX(desc.x), startY(desc.y), startWidth(desc.nWidth), startHeight(desc.nHeight)
		{}

		~wnd()
		{
			delete_child();
		}

		void delete_child()
		{
			for (auto it = map.begin(); it != map.end();)
			{
				if (it->second->hWndParent == this->handle)
				{
					wnd* w = it->second;
					it = map.erase(it);

					if (w != nullptr)
					{
						delete w;
					}
				}
				else
					it++;
			}
		}

	public:
		const DWORD		dwExStyle;
		const LPCWSTR	lpClassName;
		const LPCWSTR	lpWindowName;
		const DWORD		dwStyle;
		const HWND		hWndParent;
		const HMENU		hMenu;
		const HINSTANCE	hInstance;
		const LPVOID	lpParam;

		const int startX;
		const int startY;
		const int startWidth;
		const int startHeight;
		const HWND handle;

	private:
		static std::unordered_map<std::wstring, wnd*> map;
	public:
		static bool is_exist_(const wchar_t* name) { return map.count(name) > 0; }
		static wnd* get_(const wchar_t* name)
		{
			if (is_exist_(name) == false)
				return nullptr;

			return map[name];
		}
		static wnd* create_(const WNDDESC& desc)
		{
			if (cls::is_exist_(desc.lpClassName) == false)
				return nullptr;

			if (is_exist_(desc.lpWindowName) == true)
				return nullptr;

			HWND hwnd = CreateWindowEx
			(
				desc.dwExStyle
				, desc.lpClassName
				, desc.lpWindowName
				, desc.dwStyle
				, desc.x
				, desc.y
				, desc.nWidth
				, desc.nHeight
				, desc.hWndParent
				, desc.hMenu
				, desc.hInstance
				, desc.lpParam
			);

			wnd* n = new wnd(hwnd, desc);
			map.insert({ desc.lpWindowName, n });
			return n;
		}
		static bool delete_(const wchar_t* name)
		{
			if (is_exist_(name) == false)
				return false;

			wnd* w = map[name];
			map.erase(name);

			if (w != nullptr)
			{
				delete w;
			}
			return true;
		}

		static void release_()
		{
			for (auto it = map.begin(); it != map.end(); it++)
			{
				if (it->second != nullptr)
					delete it->second;
			}
			map.clear();
		}
	};
	std::unordered_map<std::wstring, wnd*> wnd::map = std::unordered_map<std::wstring, wnd*>();

	/* ======================================================================================================== */
	/* Global Functions */

	void release_all()
	{
		cls::release_();
		wnd::release_();
	}

	void show(HWND h) { ShowWindow(h, SW_NORMAL); }
	void hide(HWND h) { ShowWindow(h, SW_HIDE); }
	void mini(HWND h) { ShowWindow(h, SW_MINIMIZE); }
	void maxi(HWND h) { ShowWindow(h, SW_MAXIMIZE); }

	void get_transform(HWND h, int* x, int* y, int* width = nullptr, int* height = nullptr)
	{
		RECT r;
		GetWindowRect(h, &r);
		*x = r.left;
		*y = r.top;
		if(width != nullptr)
			*width = r.right - r.left;
		if(height != nullptr)
			*height = r.bottom - r.top;
	}

	void get_title(HWND h, std::wstring& s)
	{
		wchar_t wc[CHAR_MAX];
		GetWindowText(h, wc, CHAR_MAX);
		s.assign(wc);
	}

	void set_position(HWND h, const int& x, const int& y) { SetWindowPos(h, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
	void set_size(HWND h, const int& width, const int& height) { SetWindowPos(h, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER); }
	void set_zorder(HWND h, const bool& top_true_or_bottom_false) { SetWindowPos(h, (top_true_or_bottom_false ? HWND_TOP : HWND_BOTTOM), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); }
	void set_title(HWND h, std::wstring& s) { SetWindowText(h, s.c_str()); }

	void update(HWND h) { UpdateWindow(h); }


	/* ======================================================================================================== */
	/* CLASS */

	void desc_init(WNDCLASSDESC* wndclsdesc)
	{
		if (wndclsdesc == nullptr)
			return;

		memset(wndclsdesc, 0, sizeof(WNDCLASSDESC));

		wndclsdesc->cbClsExtra = 0;
		wndclsdesc->cbWndExtra = 0;
		wndclsdesc->cbSize = sizeof(WNDCLASSDESC);
		wndclsdesc->hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclsdesc->hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wndclsdesc->hIconSm = wndclsdesc->hIcon;
		wndclsdesc->lpszMenuName = NULL;
		wndclsdesc->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

		/* 남은 설정
		UINT        style;
		WNDPROC     lpfnWndProc;
		HINSTANCE   hInstance;
		LPCWSTR     lpszClassName;
		*/
	}

	WNDCLASSDESC default_class_desc(const wchar_t* name, HINSTANCE hInstance, WNDPROC proc, int background_color = WHITE_BRUSH)
	{
		WNDCLASSDESC cl;
		desc_init(&cl);

		cl.hbrBackground = (HBRUSH)GetStockObject(background_color);
		cl.hInstance = hInstance;
		cl.lpfnWndProc = proc;
		cl.lpszClassName = name;
		cl.style = CS_HREDRAW | CS_VREDRAW;

		return cl;
	}

	/* ======================================================================================================== */
	/* WINDOW */

	void desc_init(WNDDESC* wnddesc)
	{
		if (wnddesc == nullptr)
			return;

		memset(wnddesc, 0, sizeof(WNDDESC));

		wnddesc->dwExStyle = WS_EX_APPWINDOW;
		wnddesc->hWndParent = NULL;
		wnddesc->hMenu = (HMENU)NULL;
		wnddesc->lpParam = NULL;

		//	기본 설정임!
		wnddesc->nWidth = 1280;
		wnddesc->nHeight = 720;

		/* 남은 설정
		LPCWSTR		lpClassName;
		LPCWSTR		lpWindowName;
		DWORD		dwStyle;
		HINSTANCE	hInstance;
		*/
	}

	WNDDESC default_window_desc(const wchar_t* name, const wchar_t* class_name, HINSTANCE hInstance, DWORD style)
	{
		WNDDESC wn;
		desc_init(&wn);

		wn.lpClassName = class_name;
		wn.lpWindowName = name;
		wn.hInstance = hInstance;
		wn.dwStyle = style;

		return wn;
	}

	WNDDESC main_window_desc(const wchar_t* name, const wchar_t* class_name, HINSTANCE hInstance)
	{
		return default_window_desc(name, class_name, hInstance, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW);
	}


}