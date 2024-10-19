#include "Engine.h"
#include <iostream>

Engine::Engine(int screenWidth, int screenHeight, const char* title)
    : screenWidth(screenWidth), screenHeight(screenHeight), currentState(GameState::MENU) {
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60);
}

Engine::~Engine() {
    CloseWindow();
}

void Engine::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

void Engine::HandleInput() {
    switch (currentState) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = GameState::PLANET_VIEW;
            }
            break;
        case GameState::PLANET_VIEW:
            if (IsKeyPressed(KEY_C)) {
                currentState = GameState::COLONY_VIEW;
            }
            break;
        case GameState::COLONY_VIEW:
            if (IsKeyPressed(KEY_S)) {
                currentState = GameState::SECT_VIEW;
            }
            if (IsKeyPressed(KEY_P)) {
                currentState = GameState::PLANET_VIEW;
            }
            break;
        case GameState::SECT_VIEW:
            if (IsKeyPressed(KEY_U)) {
                currentState = GameState::UNIT_VIEW;
            }
            if (IsKeyPressed(KEY_C)) {
                currentState = GameState::COLONY_VIEW;
            }
            break;
        case GameState::UNIT_VIEW:
            if (IsKeyPressed(KEY_S)) {
                currentState = GameState::SECT_VIEW;
            }
            break;
    }
}

void Engine::Update() {
    // Add game logic updates here
}

void Engine::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentState) {
        case GameState::MENU:
            DrawText("COLONY", screenWidth/2 - MeasureText("COLONY", 60)/2, screenHeight/3, 60, BLACK);
            DrawText("Press ENTER to start", screenWidth/2 - MeasureText("Press ENTER to start", 20)/2, screenHeight/2, 20, GRAY);
            break;
        case GameState::PLANET_VIEW:
            DrawText("Planet View", 10, 10, 20, BLACK);
            DrawText("Press C for Colony View", 10, 40, 20, GRAY);
            break;
        case GameState::COLONY_VIEW:
            DrawText("Colony View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            DrawText("Press P for Planet View", 10, 70, 20, GRAY);
            break;
        case GameState::SECT_VIEW:
            DrawText("Sect View", 10, 10, 20, BLACK);
            DrawText("Press U for Unit View", 10, 40, 20, GRAY);
            DrawText("Press C for Colony View", 10, 70, 20, GRAY);
            break;
        case GameState::UNIT_VIEW:
            DrawText("Unit View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            break;
    }

    EndDrawing();
}
