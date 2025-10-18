#include <pch.h>
#include "hooks.h"
#include <MinHook.h>
#include <render/render.h>

INT64 UpdateWeaponAmmoHook(INT64 a1, int a2, int a3)
{
	return hooks::UpdateWeaponAmmo(a1, Settings.inf_ammo ? 0 : a2, a3);
}

INT64 ExecuteCommandHook(__int64 a1, const char* a2, __int64 a3, int a4, __int64 a5, int a6, int a7, __int64 a8, __int64 a9)
{
	//std::cout << "ExecCmdhk Called | " << a1 << " - " << a2 << " - " << (uintptr_t*)a3 << " - " << a4 << " - " << a6 << " - " << a7 << " - " << a8 << " - " << a9 << std::endl;

	__int64 cmd = 0;
	if (std::strcmp(a2, "g_weaponkick") == 0)
	{
		cmd = Settings.no_recoil ? reinterpret_cast<__int64>("0") : reinterpret_cast<__int64>("1");
	}
	else if (std::strcmp(a2, "g_weaponSpreadScale") == 0)
	{
		cmd = Settings.no_spread ? reinterpret_cast<__int64>("0") : reinterpret_cast<__int64>("1");
	}
	else if (std::strcmp(a2, "pm_noBob"))
	{
		cmd = Settings.no_sway ? reinterpret_cast<__int64>("0") : reinterpret_cast<__int64>("1");
	}

	return hooks::ExecuteCommand(a1, a2, cmd == 0 ? a3 : cmd, a4, a5, a6, a7, a8, a9);
}

char HealthControllerHook(__int64 a1, float a2, char a3, float* a4, char a5)
{
	if (Settings.inf_health)
	{
		a2 = 0.f;
		*a4 = 0.f;
	}

	//std::cout << "HealthControllerHook Called | " << a1 << " - " << a2 << " - " << (int)a3 << " - " << *a4 << " - " << (int)a5 << std::endl;
	return hooks::HealthController(a1, a2, a3, a4, a5);
}


void hooks::initialize()
{
	MH_Initialize();
	auto MainModule = GetModuleHandleA(NULL);

	UpdateWeaponAmmo = utils::GetPatternAddress<UpdateAmmo>(MainModule, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x00\x01\x51", "xxxx?xxxx?xx");
	ExecuteCommand = utils::GetPatternAddress<ExecuteCmd>(MainModule, "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x48\x83\xEC\x00\x45\x33\xE4\x41\x8B\xF9", "xxxx?xxxx?xxxx?xxxx?xxxxx?xxxxxx");

	HealthController = reinterpret_cast<HealthControl>(BaseAddress + 0x641850);

	glSwapBuffers = reinterpret_cast<TWglSwapBuffers>(GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers"));

	internal::NoRecoil = reinterpret_cast<fn>(BaseAddress + 0xBB4860);
	internal::NoSpread = reinterpret_cast<fn>(BaseAddress + 0xBB4AB0);
	internal::NoSway = reinterpret_cast<fn>(BaseAddress + 0xBB6780);

	MH_CreateHook(glSwapBuffers, &render::Draw, (LPVOID*)&glSwapBuffers);

	MH_CreateHook(UpdateWeaponAmmo, &UpdateWeaponAmmoHook, (LPVOID*)&UpdateWeaponAmmo);
	MH_CreateHook(ExecuteCommand, &ExecuteCommandHook, (LPVOID*)&ExecuteCommand);
	MH_CreateHook(HealthController, &HealthControllerHook, (LPVOID*)&HealthController);

	MH_EnableHook(MH_ALL_HOOKS);

	Sleep(2500);

	{
		internal::NoRecoil();
		internal::NoSpread();
		internal::NoSway();
	}
	
}

void hooks::shutdown()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
}