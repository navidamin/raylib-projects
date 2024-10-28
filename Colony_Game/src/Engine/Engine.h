#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include "raymath.h"
#include "planet.h"
#include "colony.h"
#include "sect.h"
#include "unit.h"
#include <vector>

enum class View {
    Menu,
    Planet,
    Colony,
    Sect,
    Unit
};

class Engine {
public:
    Engine(int screenWidth, int screenHeight, const char* title);
    ~Engine();

    void InitGame();
    void Run();

private:
    void HandleInput();
    void Update();
    void Draw();

    bool IsDoubleClick();
    void SwitchToColonyView();
    void SwitchToSectView();
    void SwitchToUnitView();
    void SwitchToPlanetView();
    void SelectColony(Vector2 mousePosition);
    void SelectSect(Vector2 mousePosition);
    void SelectUnit(Vector2 mousePosition);

    Camera2D camera;
    int screenWidth;
    int screenHeight;
    View currentView;  // Changed from currentState to currentView

    Planet* planet;
    std::vector<Colony*> colonies;
    Colony* currentColony;
    Sect* currentSect;
    Unit* currentUnit;
    // Double-click detection
    double lastClickTime;
    Vector2 lastClickPosition;
};

#endif // ENGINE_H
