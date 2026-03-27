#ifndef PAUSE_MENU
#define PAUSE_MENU

struct PauseMenu {
	static void Load();
	static void Init();
	static void Update();
	static void Render();
	static void Free();
	static void Unload();
};

struct ConfirmationMenu {
	static void Init();
	static void Update();
	static void Render();
};

#endif
