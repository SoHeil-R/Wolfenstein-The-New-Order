#include <pch.h>
#include <gl/GL.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_opengl2.h>
#include <hooks/hooks.h>
#pragma comment(lib, "opengl32.lib")

#include "render.h"

LRESULT __stdcall render::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (MenuShow)
        return 0;

    return CallWindowProcA(render::og_wndproc, hWnd, uMsg, wParam, lParam);
}

void LoadKeys()
{
    ImGuiIO& io = ImGui::GetIO();

    io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
    io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
    io.MouseDown[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000;
    io.MouseDown[3] = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;
    io.MouseDown[4] = GetAsyncKeyState(VK_XBUTTON2) & 0x8000;

    bool isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
    bool isCaps = GetKeyState(0x14);
    for (int i = '0'; i < '9'; i++) {
        if (GetAsyncKeyState(i) & 1) {
            io.AddInputCharacter(i);
        }
    }
    for (int i = 'A'; i < 'Z'; i++) {
        if (GetAsyncKeyState(i) & 1) {
            if (isCaps && !isShift) {
                io.AddInputCharacter(i);
            }
            else if (isShift && !isCaps) {
                io.AddInputCharacter(i);
            }
            else {
                io.AddInputCharacter(i + 32);
            }
        }
    }
}

bool __stdcall render::Draw(HDC hDc)
{
    HGLRC origin_context{ wglGetCurrentContext() };
    static HGLRC new_context{};

    if (static bool was_init{}; was_init == false)
    {
        new_context = wglCreateContext(hDc);
        wglMakeCurrent(hDc, new_context);

		window = WindowFromDC(hDc);
        og_wndproc = (WNDPROC)SetWindowLongPtrW(window, GWLP_WNDPROC, (LONG_PTR)render::WndProc);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, viewport[2], viewport[3]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);

        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(window);
        ImGui_ImplOpenGL2_Init();
        was_init = true;
    }
    else
    {
        wglMakeCurrent(hDc, new_context);

        if (GetAsyncKeyState(VK_INSERT) & 0x0001)
			MenuShow = !MenuShow;

        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = MenuShow;

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        LoadKeys();

        if (MenuShow)
        {
            ImGui::Begin("Wolfenstein: The New Order");
            {
                ImGui::Checkbox("God mode", &Settings.inf_health);
                ImGui::Checkbox("Infinite Ammo", &Settings.inf_ammo);

                if (ImGui::Checkbox("No Recoil", &Settings.no_recoil))
                {
                    hooks::internal::NoRecoil();
                }

                if (ImGui::Checkbox("No Spread", &Settings.no_spread))
                {
                    hooks::internal::NoSpread();
                }

                if (ImGui::Checkbox("No Sway", &Settings.no_sway))
                {
                    hooks::internal::NoSway();
                }


            }
            ImGui::End();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    wglMakeCurrent(hDc, origin_context);


    return hooks::glSwapBuffers(hDc);
}

