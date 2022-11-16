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
#include <functional>

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

		WNDPROC		proc;

		int x;
		int y;
		int nWidth;
		int nHeight;
	}WNDDESC;

	LRESULT CALLBACK improc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK emptyproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return LRESULT(); }
	LRESULT CALLBACK mainproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { if (message == WM_DESTROY) { PostQuitMessage(0); } return 0; }

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
			, proc(desc.proc), startX(desc.x), startY(desc.y), startWidth(desc.nWidth), startHeight(desc.nHeight)
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

		const WNDPROC	proc;

		const int startX;
		const int startY;
		const int startWidth;
		const int startHeight;

		const HWND handle;

	private:
		static std::unordered_map<std::wstring, wnd*> map;
		static std::unordered_map<HWND, std::wstring> handle_map;
	public:
		static bool is_exist_(const wchar_t* name) { return map.count(name) > 0; }
		static bool is_exist_(HWND h) { return handle_map.count(h) > 0; }
		static wnd* get_(const wchar_t* name)
		{
			if (is_exist_(name) == false)
				return nullptr;

			return map[name];
		}
		static wnd* get_(HWND h)
		{
			if (is_exist_(h) == false)
				return nullptr;

			return map[handle_map[h]];
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
			assert(hwnd != 0);

			handle_map.insert({ hwnd, desc.lpWindowName });

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
			handle_map.erase(w->handle);

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
			handle_map.clear();
		}
	};
	std::unordered_map<std::wstring, wnd*> wnd::map = std::unordered_map<std::wstring, wnd*>();
	std::unordered_map<HWND, std::wstring> wnd::handle_map = std::unordered_map<HWND, std::wstring>();

	/* ======================================================================================================== */
	/* Global Functions */

	void release_all()
	{
		cls::release_();
		wnd::release_();
	}

	/* ========================================================================================================= */
	/* GET */

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
	inline void get_transform(wnd* w, int* x, int* y, int* width = nullptr, int* height = nullptr) { get_transform(w->handle, x, y, width, height); }

	void get_title(HWND h, std::wstring* s)
	{
		wchar_t wc[CHAR_MAX];
		GetWindowText(h, wc, CHAR_MAX);
		s->clear();
		s->assign(wc);
	}
	inline void get_title(wnd* w, std::wstring* s) { get_title(w->handle, s); }

	int get_show_state(HWND h)
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
	inline int get_show_state(wnd* w) { return get_show_state(w->handle); }

	/* ========================================================================================================= */
	/* SET */

	void set_position(HWND h, const int& x, const int& y) { SetWindowPos(h, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
	void set_size(HWND h, const int& width, const int& height) { SetWindowPos(h, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER); }
	void set_zorder(HWND h, const bool& top_true_or_bottom_false) { SetWindowPos(h, (top_true_or_bottom_false ? HWND_TOP : HWND_BOTTOM), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); }
	void set_title(HWND h, std::wstring& s) { SetWindowText(h, s.c_str()); }

	inline void set_position(wnd* w, const int& x, const int& y) { set_position(w->handle, x, y); }
	inline void set_size(wnd* w, const int& width, const int& height) { set_size(w->handle, width, height); }
	inline void set_zorder(wnd* w, const bool& top_true_or_bottom_false) { set_zorder(w->handle,top_true_or_bottom_false); }
	inline void set_title(wnd* w, std::wstring& s) { set_title(w->handle, s); }

	/* ========================================================================================================= */
	/* STATE */

	void show(HWND h) { ShowWindow(h, SW_NORMAL); }
	void hide(HWND h) { ShowWindow(h, SW_HIDE); }
	void mini(HWND h) { ShowWindow(h, SW_MINIMIZE); }
	void maxi(HWND h) { ShowWindow(h, SW_MAXIMIZE); }
	inline void show(wnd* w) { show(w->handle); }
	inline void hide(wnd* w) { hide(w->handle); }
	inline void mini(wnd* w) { mini(w->handle); }
	inline void maxi(wnd* w) { maxi(w->handle); }

	bool is_show(HWND h) { return get_show_state(h) == SW_SHOW; }
	bool is_hide(HWND h) { return get_show_state(h) == SW_HIDE; }
	bool is_mini(HWND h) { return get_show_state(h) == SW_MINIMIZE; }
	bool is_maxi(HWND h) { return get_show_state(h) == SW_MAXIMIZE; }
	inline bool is_show(wnd* w) { return get_show_state(w->handle) == SW_SHOW; }
	inline bool is_hide(wnd* w) { return get_show_state(w->handle) == SW_HIDE; }
	inline bool is_mini(wnd* w) { return get_show_state(w->handle) == SW_MINIMIZE; }
	inline bool is_maxi(wnd* w) { return get_show_state(w->handle) == SW_MAXIMIZE; }

	void update(HWND h) { UpdateWindow(h); }
	inline void update(wnd* w) { update(w->handle); }

	void enable(HWND h) { EnableWindow(h, TRUE); }
	void disable(HWND h) { EnableWindow(h, FALSE); }
	bool is_enable(HWND h) { return IsWindowEnabled(h); }
	bool is_disable(HWND h) { return !IsWindowEnabled(h); }
	inline void enable(wnd* w) { enable(w->handle); }
	inline void disable(wnd* w) { disable(w->handle); }
	inline bool is_enable(wnd* w) { return is_enable(w->handle); }
	inline bool is_disable(wnd* w) { return is_disable(w->handle); }

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
		wndclsdesc->lpfnWndProc = improc;

		/* 남은 설정
		UINT        style;
		HINSTANCE   hInstance;
		LPCWSTR     lpszClassName;
		*/
	}

	WNDCLASSDESC default_class_desc(const wchar_t* name, HINSTANCE hInstance, int background_color = WHITE_BRUSH)
	{
		WNDCLASSDESC cl;
		desc_init(&cl);

		cl.hbrBackground = (HBRUSH)GetStockObject(background_color);
		cl.hInstance = hInstance;
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

		wnddesc->proc = emptyproc;

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

	WNDDESC default_window_desc(const wchar_t* name, const wchar_t* class_name, HINSTANCE hInstance, DWORD style, WNDPROC proc = emptyproc)
	{
		WNDDESC wn;
		desc_init(&wn);

		wn.lpClassName = class_name;
		wn.lpWindowName = name;
		wn.hInstance = hInstance;
		wn.dwStyle = style;
		wn.proc = proc;

		return wn;
	}

	WNDDESC main_window_desc(const wchar_t* name, const wchar_t* class_name, HINSTANCE hInstance)
	{
		return default_window_desc(name, class_name, hInstance, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, mainproc);
	}



	/* PROC */
	LRESULT CALLBACK improc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		wnd* w = wnd::get_(hWnd);
		if (w != nullptr)
			w->proc(hWnd, message, wParam, lParam);

		switch (message)
		{
		case WM_DESTROY:
			if (w != nullptr)
				wnd::delete_(w->lpWindowName);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

}