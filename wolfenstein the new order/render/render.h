#pragma once

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//static LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace render
{
	inline bool MenuShow{ true };

	inline void* p_swap_buffers{};
	inline HWND window{};
	inline WNDPROC og_wndproc{};

	bool __stdcall Draw(HDC hDc);
	LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}