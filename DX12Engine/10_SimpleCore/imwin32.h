#pragma once

namespace imwin32
{
	/* ======================================================================================================== */
	/* Global Functions */

	HWND create_main_window(HINSTANCE hInstance, WNDPROC proc, int width, int height, const wchar_t* title, void* attach = nullptr);

	/* ========================================================================================================= */
	/* GET */

	template<typename T>
	T* get_attach(HWND h)
	{
		return reinterpret_cast<T*>(GetWindowLongPtr(h, GWLP_USERDATA));
	}

	void get_position(HWND h, int* x, int* y);
	void get_size(HWND h, int* width, int* height);
	void get_title(HWND h, std::wstring* s);
	int get_show_state(HWND h);

	/* ========================================================================================================= */
	/* SET */

	void set_position(HWND h, const int& x, const int& y);
	void set_size(HWND h, const int& width, const int& height);
	void set_zorder(HWND h, const bool& top_true_or_bottom_false);
	void set_title(HWND h, std::wstring& s);

	/* ========================================================================================================= */
	/* STATE */

	void show(HWND h) { ShowWindow(h, SW_NORMAL); }
	void hide(HWND h) { ShowWindow(h, SW_HIDE); }
	void mini(HWND h) { ShowWindow(h, SW_MINIMIZE); }
	void maxi(HWND h) { ShowWindow(h, SW_MAXIMIZE); }

	bool is_show(HWND h) { return get_show_state(h) == SW_SHOW; }
	bool is_hide(HWND h) { return get_show_state(h) == SW_HIDE; }
	bool is_mini(HWND h) { return get_show_state(h) == SW_MINIMIZE; }
	bool is_maxi(HWND h) { return get_show_state(h) == SW_MAXIMIZE; }

	void update(HWND h) { UpdateWindow(h); }

	void enable(HWND h) { EnableWindow(h, TRUE); }
	void disable(HWND h) { EnableWindow(h, FALSE); }
	bool is_enable(HWND h) { return IsWindowEnabled(h); }
	bool is_disable(HWND h) { return !IsWindowEnabled(h); }

}