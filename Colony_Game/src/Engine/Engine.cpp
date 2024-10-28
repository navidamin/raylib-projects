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
    // Create initial colony
    Colony* firstColony = new Colony();
    colonies.push_back(firstColony);
    currentColony = firstColony;

    // Create initial sect with a position near the center of the map
    Sect* firstSect = new Sect();
    Vector2 initialPosition = {
        static_cast<float>(screenWidth) / 2.0f,
        static_cast<float>(screenHeight) / 2.0f
    };
    firstSect->SetPosition(initialPosition);

    // Add sect to colony
    currentColony->AddSect(firstSect);
    currentSect = firstSect;

    // Initialize camera to focus on the first sect
    camera.target = initialPosition;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Set initial view to Colony view to see the first sect
    currentView = View::Colony;
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
        if (Vector2Distance(mousePosition, colony->GetCentroid()) <= colony->GetRadius()) {
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
            if (Vector2Distance(mousePosition, unit->GetUnitPosInSectView()) <= unit->GetUnitRadiusInSectView()) {
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

void Engine::Update() {
    // For now, we can leave it empty or add basic update logic
    /*
    if (currentColony) {
        currentColony->Update();
    }

    if (currentSect) {
        currentSect->Update();
    }

    if (currentUnit) {
        currentUnit->Update();
    }*/
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
            planet->Draw(camera.zoom);
            DrawText("Planet View", 10, 10, 20, BLACK);
            DrawText("Press C for Colony View", 10, 40, 20, GRAY);
            break;
        case View::Colony:
            if (currentColony) {
                for (auto& sect : currentColony->GetSects()) {
                    sect->DrawInColonyView(sect->GetPosition(), camera.zoom);
                }
            }
            DrawText("Colony View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            DrawText("Press P for Planet View", 10, 70, 20, GRAY);
            break;
        case View::Sect:
            if (currentSect) {
                currentSect->DrawInSectView(Vector2{GetScreenWidth()/2.0f, GetScreenHeight()/2.0f});
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

