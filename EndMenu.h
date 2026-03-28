#ifndef END_MENU_H
#define END_MENU_H

struct EndMenu {
    static bool isActive;    // Is the pop-up currently showing?
    static bool isWin;       // True for "Congrats", False for "You Are Dead"

    // Stats Placeholders
    static int score;
    static float timeTaken;
    static int deathCounter;

    static void Init();
    static void Update();
    static void Render();
    static void Free();
};

#endif