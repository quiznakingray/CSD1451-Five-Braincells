#ifndef END_MENU_H
#define END_MENU_H

#include "SingletonTemplate.h"

struct EndMenu : Singleton<EndMenu> {
    bool isActive = true;    // Is the pop-up currently showing?
    bool isWin;       // True for "Congrats", False for "You Are Dead"

    // Stats Placeholders
    int score;
    float timeTaken;
    int deathCounter;

    void Init();
    void Update();
    void Render();
    void Free();
};

#endif