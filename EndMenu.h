#ifndef END_MENU_H
#define END_MENU_H

#include "SingletonTemplate.h"

struct EndMenu
{
    static EndMenu& GetInstance()
    {
        static EndMenu instance;
        return instance;
    }
    EndMenu(const EndMenu&) = delete;
    EndMenu& operator=(const EndMenu&) = delete;

    bool  isActive = false;
	bool  won = false;
    int   score = 0;
    float timeTaken = 0.0f;
    int   deathCounter = 0;


    void Init();
    void Update();
    void Render();
    void Free();

private:
    EndMenu() = default;
};

#endif  