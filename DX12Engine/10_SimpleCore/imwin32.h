#pragma once

namespace imwin32
{
	/* ======================================================================================================== */
	/* Global Functions */

	HWND create_default_window(HINSTANCE hInstance, WNDPROC proc, int width, int height, const wchar_t* title, void* attach = nullptr);
	UINT regist_window_class(HINSTANCE hInstance, WNDPROC proc, UINT cs_style, const wchar_t* name);
	HWND create_window(HINSTANCE hInstance, const wchar_t* class_name, int width, int height, DWORD ws_style,const wchar_t* title, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, void* attach = nullptr);
	HWND create_window(UINT class_index, int width, int height, DWORD ws_style, const wchar_t* title, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, void* attach = nullptr);

	void unregist_window_class(HINSTANCE hInstance, const wchar_t* class_name);
	void unregist_window_class(UINT class_index);

	void destroy_window(HWND handle);

	void clean();

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

	void show(HWND h);
	void hide(HWND h);
	void mini(HWND h);
	void maxi(HWND h);

	bool is_show(HWND h);
	bool is_hide(HWND h);
	bool is_mini(HWND h);
	bool is_maxi(HWND h);

	void update(HWND h);

	void enable(HWND h);
	void disable(HWND h);
	bool is_enable(HWND h);
	bool is_disable(HWND h);

}