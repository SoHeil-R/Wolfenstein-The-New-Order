#pragma once

struct setting
{
	bool inf_ammo{ true };
	bool inf_health{ true };

	bool no_recoil{ true };
	bool no_spread{ true };
	bool no_sway{ true };
};

inline setting Settings;