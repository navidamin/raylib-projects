#include "Engine.h"
#include <iostream>

Engine::Engine(int screenWidth, int screenHeight, const char* title)
    : screenWidth(screenWidth), screenHeight(screenHeight), currentView(View::Menu),
      planet(nullptr), currentColony(nullptr), currentSect(nullptr), currentUnit(nullptr),
      lastClickTime(0), lastClickPosition({0, 0}) {
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60);
}

Engine::~Engine() {
    CloseWindow();
}

void Engine::InitGame() {
    // Initialize game components
    // This is a placeholder, you'll need to implement the actual initialization logic
    std::cout << "Game initialized" << std::endl;
}

void Engine::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

bool Engine::IsDoubleClick() {
    double currentTime = GetTime();
    Vector2 currentPosition = GetMousePosition();

    bool isDoubleClick = (currentTime - lastClickTime <= 0.5) &&
                         (Vector2Distance(lastClickPosition, currentPosition) <= 10);

    lastClickTime = currentTime;
    lastClickPosition = currentPosition;

    return isDoubleClick;
}


void Engine::SwitchToColonyView() {
    if (currentColony) {
        currentView = View::Colony;
    }
}

void Engine::SwitchToSectView() {
    if (currentColony && currentSect) {
        currentView = View::Sect;
    }
}

void Engine::SwitchToUnitView() {
    if (currentColony && currentSect && currentUnit) {
        currentView = View::Unit;
    }
}

void Engine::SwitchToPlanetView() {
    currentView = View::Planet;
}

void Engine::SelectColony(Vector2 mousePosition) {
    // Logic to determine which colony was clicked
    for (auto& colony : colonies) {
        if (Vector2Distance(mousePosition, colony->GetPosition()) <= colony->GetRadius()) {
            currentColony = colony;
            SwitchToColonyView();
            break;
        }
    }
}

void Engine::SelectSect(Vector2 mousePosition) {
    // Logic to determine which sect was clicked
    if (currentColony) {
        for (auto& sect : currentColony->GetSects()) {
            if (Vector2Distance(mousePosition, sect->GetPosition()) <= sect->GetRadius()) {
                currentSect = sect;
                SwitchToSectView();
                break;
            }
        }
    }
}

void Engine::SelectUnit(Vector2 mousePosition) {
    // Logic to determine which unit was clicked
    if (currentSect) {
        for (auto& unit : currentSect->GetUnits()) {
            if (Vector2Distance(mousePosition, unit->GetPosition()) <= unit->GetRadius()) {
                currentUnit = unit;
                SwitchToUnitView();
                break;
            }
        }
    }
}

void Engine::HandleInput() {
    switch (currentView) {
        case View::Menu:
            if (IsKeyPressed(KEY_ENTER)) {
                currentView = View::Planet;
            }
            break;
        case View::Planet:
            if (IsKeyPressed(KEY_C)) {
                currentView = View::Colony;
            }
            break;
        case View::Colony:
            if (IsKeyPressed(KEY_S)) {
                currentView = View::Sect;
            }
            if (IsKeyPressed(KEY_P)) {
                currentView = View::Planet;
            }
            break;
        case View::Sect:
            if (IsKeyPressed(KEY_U)) {
                currentView = View::Unit;
            }
            if (IsKeyPressed(KEY_C)) {
                currentView = View::Colony;
            }
            break;
        case View::Unit:
            if (IsKeyPressed(KEY_S)) {
                currentView = View::Sect;
            }
            break;
    }

    // Handle double-click selection of specific colonies, sects, and units
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsDoubleClick()) {
        Vector2 mousePosition = GetMousePosition();
        switch (currentView) {
            case View::Planet:
                SelectColony(mousePosition);
                break;
            case View::Colony:
                SelectSect(mousePosition);
                break;
            case View::Sect:
                SelectUnit(mousePosition);
                break;
            case View::Unit:
                // Handle double-click in Unit view if needed
                break;
            case View::Menu:
                // Handle double-click in Menu if needed
                break;
        }
    }
}

void Engine::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentView) {
        case View::Menu:
            DrawText("COLONY", GetScreenWidth()/2 - MeasureText("COLONY", 60)/2, GetScreenHeight()/3, 60, BLACK);
            DrawText("Press ENTER to start", GetScreenWidth()/2 - MeasureText("Press ENTER to start", 20)/2, GetScreenHeight()/2, 20, GRAY);
            break;
        case View::Planet:
            planet->Draw();
            DrawText("Planet View", 10, 10, 20, BLACK);
            DrawText("Press C for Colony View", 10, 40, 20, GRAY);
            break;
        case View::Colony:
            if (currentColony) {
                currentColony->Draw();
            }
            DrawText("Colony View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            DrawText("Press P for Planet View", 10, 70, 20, GRAY);
            break;
        case View::Sect:
            if (currentSect) {
                currentSect->Draw(Vector2{GetScreenWidth()/2.0f, GetScreenHeight()/2.0f});
            }
            DrawText("Sect View", 10, 10, 20, BLACK);
            DrawText("Press U for Unit View", 10, 40, 20, GRAY);
            DrawText("Press C for Colony View", 10, 70, 20, GRAY);
            break;
        case View::Unit:
            if (currentUnit) {
                currentUnit->DrawInUnitView();
            }
            DrawText("Unit View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            break;
    }

    DrawText("Double-click to select", 10, GetScreenHeight() - 30, 20, DARKGRAY);

    EndDrawing();
}

