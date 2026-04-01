#ifndef END_MENU_H
#define END_MENU_H

#include "SingletonTemplate.h"

struct EndMenu  {
    bool isActive = true;    // Is the pop-up currently showing?
    bool isWin = false;       // True for "Congrats", False for "You Are Dead"

    // Stats Placeholders
    int score = 0;
    float timeTaken = 0.0f;
    int deathCounter = 0;

    void Init();
    void Update();
    void Render();
    void Free();

};

#endif