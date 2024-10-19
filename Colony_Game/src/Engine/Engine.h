#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"

// Game states
enum class GameState {
    MENU,
    PLANET_VIEW,
    COLONY_VIEW,
    SECT_VIEW,
    UNIT_VIEW
};

class Engine {
public:
    Engine(int screenWidth, int screenHeight, const char* title);
    ~Engine();

    void Run();

private:
    void Update();
    void Draw();
    void HandleInput();

    int screenWidth;
    int screenHeight;
    GameState currentState;
};

#endif // ENGINE_H
