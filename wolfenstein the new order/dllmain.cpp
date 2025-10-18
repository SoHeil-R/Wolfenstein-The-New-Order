#include <pch.h>
#include <hooks/hooks.h>
#include <render/render.h>

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);
    std::cout << "Loaded" << std::endl;

    BaseAddress = GetModuleHandle(NULL);

    std::cout << "Base Address: " << (uintptr_t*)BaseAddress << std::endl;

	hooks::initialize();


    while (true)
    {
        if (GetAsyncKeyState(VK_RSHIFT) & 0x8000 && GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
            std::cout << "Unloaded!" << std::endl;
            break;
        }
        Sleep(1);
    }

	hooks::shutdown();

    SetWindowLongPtrW(render::window, GWLP_WNDPROC, (LONG_PTR)render::og_wndproc);

    FreeLibraryAndExitThread(Module, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

