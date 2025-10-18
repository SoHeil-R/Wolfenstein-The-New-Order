#pragma once


using fn = void(*)();
typedef BOOL(__stdcall* TWglSwapBuffers) (HDC hDc);
typedef INT64(__fastcall* UpdateAmmo)(INT64 a1, int a2, int a3);
typedef INT64(__fastcall* ExecuteCmd)(__int64 a1, const char* a2, __int64 a3, int a4, __int64 a5, int a6, int a7, __int64 a8, __int64 a9);
typedef char(__fastcall* HealthControl)(__int64 a1, float a2, char a3, float* a4, char a5);

namespace hooks
{
	inline ExecuteCmd ExecuteCommand{};
	inline UpdateAmmo UpdateWeaponAmmo{};
	inline HealthControl HealthController{};
	inline TWglSwapBuffers glSwapBuffers{};

	namespace internal
	{
		inline fn NoRecoil;
		inline fn NoSpread;
		inline fn NoSway;
	}

	void initialize();
	void shutdown();
}