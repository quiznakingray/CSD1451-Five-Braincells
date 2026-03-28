#ifndef GAME_STATE_MANAGER
#define GAME_STATE_MANAGER

typedef void(*FP)(void);
extern FP fpLoad , fpInitialize , fpUpdate , fpRender , fpFree , fpUnload ;

enum class GAME_STATE_TYPE {
	MENU,
	WORLD,
	OTHER,
	COMBAT,
	INSTRUCTIONS,
	SETTING,
	CREDITS
};
extern GAME_STATE_TYPE current, next;
extern GAME_STATE_TYPE previousState; // To remember where to "Resume" to
extern GAME_STATE_TYPE pendingAction; // To remember if we are confirming "Restart" or "Main Menu"


struct GameState {

	virtual void Load(){};
	virtual void Init(){};
	virtual void Update(){};
	virtual void Render(){};
	virtual void Free(){};
	virtual void Unload(){};
};

struct GameStateManager {

	void Initialize(GAME_STATE_TYPE type);

	void ChangeState(GAME_STATE_TYPE type);

	void Update();
};
#endif // !GAME_STATE_MANAGER
