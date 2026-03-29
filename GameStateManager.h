#ifndef GAME_STATE_MANAGER
#define GAME_STATE_MANAGER

#include "SingletonTemplate.h"
typedef void(*FP)(void);
extern FP fpLoad , fpInitialize , fpUpdate , fpRender , fpFree , fpUnload ;

enum class GAME_STATE_TYPE {
	MENU,
	INSTRUCTIONS,
	LEVEL1,
	LEVEL1BOSS,
	LEVEL2,
	LEVEL2BOSS,
	LEVEL3,
	SETTING,
	//PAUSE,
	//CONFIRMATION,
	RESTART,
	CREDITS
};
extern GAME_STATE_TYPE current, next, previous; // To remember where to "Resume" to
extern GAME_STATE_TYPE pendingAction; // To remember if we are confirming "Restart" or "Main Menu"


struct GameState {

	virtual void Load(){};
	virtual void Init(){};
	virtual void Update(){};
	virtual void Render(){};
	virtual void Free(){};
	virtual void Unload(){};
};

struct GameStateManager :Singleton<GameStateManager>{

	bool showPauseMenu = false;
	void Initialize(GAME_STATE_TYPE type);

	GAME_STATE_TYPE GetCurrentState();

	void ChangeState(GAME_STATE_TYPE type);

	void Update();
};
#endif // !GAME_STATE_MANAGER
