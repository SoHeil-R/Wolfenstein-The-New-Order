#include <pch.h>
#include "hooks.h"
#include <MinHook.h>
#include <render/render.h>

INT64 UpdateWeaponAmmoHook(INT64 a1, int a2, int a3)
{
	std::cout << Settings.inf_ammo << std::endl;
	return hooks::UpdateWeaponAmmo(a1, Settings.inf_ammo ? 0 : a2, a3);
}

INT64 ExecuteCommandHook(__int64 a1, const char* a2, __int64 a3, int a4, __int64 a5, int a6, int a7, __int64 a8, __int64 a9)
{
	std::cout << "ExecCmdhk Called | " << a1 << " - " << a2 << " - " << (uintptr_t*)a3 << " - " << a4 << " - " << a6 << " - " << a7 << " - " << a8 << " - " << a9 << std::endl;
	std::string cmd;
	if (std::strcmp(a2, "g_weaponkick") == 0)
	{
		cmd = Settings.no_recoil ? "0" : "1";
	}
	return hooks::ExecuteCommand(a1, a2, cmd.empty() ? a3 : reinterpret_cast<__int64>(cmd.c_str()), a4, a5, a6, a7, a8, a9);
}

void hooks::initialize()
{
	MH_Initialize();

	UpdateWeaponAmmo = utils::GetPatternAddress<UpdateAmmo>(BaseAddress, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x00\x01\x51", "xxxx?xxxx?xx");
	ExecuteCommand = utils::GetPatternAddress<ExecuteCmd>(BaseAddress, "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x48\x83\xEC\x00\x45\x33\xE4\x41\x8B\xF9", "xxxx?xxxx?xxxx?xxxx?xxxxx?xxxxxx");

	glSwapBuffers = reinterpret_cast<TWglSwapBuffers>(GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers"));

	internal::NoRecoil = reinterpret_cast<fn>(BaseAddress + 0xBB4860);

	MH_CreateHook(glSwapBuffers, &render::Draw, (LPVOID*)&glSwapBuffers);

	MH_CreateHook(UpdateWeaponAmmo, &UpdateWeaponAmmoHook, (LPVOID*)&UpdateWeaponAmmo);
	MH_CreateHook(ExecuteCommand, &ExecuteCommandHook, (LPVOID*)&ExecuteCommand);

	MH_EnableHook(MH_ALL_HOOKS);
}

void hooks::shutdown()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
}