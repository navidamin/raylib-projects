#include "Engine.h"

#include <iostream>

Engine::Engine(int screenWidth, int screenHeight, const char* title)
    : screenWidth(screenWidth),
      screenHeight(screenHeight),
      currentView(View::Menu),
      planet(nullptr),
      currentColony(nullptr),
      currentSect(nullptr),
      currentUnit(nullptr),
      lastClickTime(0),
      lastClickPosition({0, 0}),
      minZoom(0.5f),
      maxZoom(2.0f),
      isDragging(false)
{
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60);

    // Initialize camera
    camera.target = {0, 0};
    camera.offset = {static_cast<float>(screenWidth)/2, static_cast<float>(screenHeight)/2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
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
    Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, camera);

    for (auto& colony : colonies) {
        if (Vector2Distance(worldMousePos, colony->GetCentroid()) <= colony->GetRadius()) {
            currentColony = colony;
            SwitchToColonyView();
            break;
        }
    }
}

void Engine::SelectSect(Vector2 mousePosition) {
    // Logic to determine which sect was clicked
    if (currentColony) {
        // Convert screen coordinates to world coordinates using the camera
        Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, camera);

        for (auto& sect : currentColony->GetSects()) {
            Vector2 sectPos = sect->GetPosition();
            float sectRadius = sect->GetRadius();

            if (Vector2Distance(worldMousePos, sectPos) <= sectRadius) {
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

    HandleCameraControls();  // Always handle camera controls first

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

void Engine::HandleCameraControls() {
    // Mouse wheel zooming
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        // Get world point before zoom
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Modify zoom
        camera.zoom += wheel * 0.1f;
        camera.zoom = Clamp(camera.zoom, minZoom, maxZoom);

        // Get world point after zoom
        Vector2 mouseWorldPosNew = GetScreenToWorld2D(GetMousePosition(), camera);

        // Adjust camera target to zoom into mouse position
        camera.target.x += (mouseWorldPos.x - mouseWorldPosNew.x);
        camera.target.y += (mouseWorldPos.y - mouseWorldPosNew.y);
    }

    // Pan with middle mouse button
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
        dragStart = GetMousePosition();
        isDragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
        isDragging = false;
    }

    if (isDragging) {
        Vector2 delta = GetMouseDelta();
        camera.target.x -= delta.x / camera.zoom;
        camera.target.y -= delta.y / camera.zoom;
    }

    // Reset view based on current mode
    if (IsKeyPressed(KEY_R)) {
        ResetCameraForCurrentView();
    }
}

void Engine::ResetCameraForCurrentView() {
    switch (currentView) {
        case View::Planet: {
            // Show the entire planet
            float planetWidth = SECT_CORE_RADIUS * PLANET_SIZE * 2;
            float planetHeight = SECT_CORE_RADIUS * PLANET_SIZE * 2;

            // Calculate zoom to fit planet
            float zoomX = screenWidth / planetWidth;
            float zoomY = screenHeight / planetHeight;
            camera.zoom = std::min(zoomX, zoomY) * 0.9f; // 90% to add some padding

            // Center on planet
            camera.target = {planetWidth/2, planetHeight/2};
            break;
        }
        case View::Colony: {
            if (currentColony) {
                // Center on colony
                camera.target = currentColony->GetCentroid();
                camera.zoom = 1.0f; // Or whatever zoom level works best for colony view
            }
            break;
        }
        default:
            break;
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
        case View::Planet:{
            BeginMode2D(camera);

            // Draw planet grid
            float planetSize = SECT_CORE_RADIUS * PLANET_SIZE * 2;
            for (int i = 0; i <= PLANET_SIZE; i++) {
                float linePos = i * SECT_CORE_RADIUS * 2;
                DrawLineV({linePos, 0}, {linePos, planetSize}, LIGHTGRAY);
                DrawLineV({0, linePos}, {planetSize, linePos}, LIGHTGRAY);
            }

            // Draw all colonies
            for (const auto& colony : colonies) {
                colony->Draw(camera.zoom);
            }

            EndMode2D();

            DrawText("Planet View", 10, 10, 20, BLACK);
            DrawText("Press C for Colony View", 10, 40, 20, GRAY);
            break;
        }


        case View::Colony:{
            // Start drawing with camera transformation
            BeginMode2D(camera);

            if (currentColony) {
                // Calculate visible area in world coordinates
                Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
                Vector2 bottomRight = GetScreenToWorld2D(
                    {static_cast<float>(screenWidth),
                     static_cast<float>(screenHeight)},
                    camera
                );

                // Calculate which grid lines are visible
                int startX = std::max(0, static_cast<int>(topLeft.x / (SECT_CORE_RADIUS * 2)));
                int endX = std::min(PLANET_SIZE, static_cast<int>(bottomRight.x / (SECT_CORE_RADIUS * 2)) + 1);
                int startY = std::max(0, static_cast<int>(topLeft.y / (SECT_CORE_RADIUS * 2)));
                int endY = std::min(PLANET_SIZE, static_cast<int>(bottomRight.y / (SECT_CORE_RADIUS * 2)) + 1);

                // Draw only visible vertical grid lines
                for (int i = startX; i <= endX; i++) {
                    float linePos = i * SECT_CORE_RADIUS * 2;
                    DrawLineV(
                        {linePos, topLeft.y},     // Start from top of screen
                        {linePos, bottomRight.y},  // End at bottom of screen
                        Fade(LIGHTGRAY, 0.5f)     // Semi-transparent gray
                    );
                }

                // Draw only visible horizontal grid lines
                for (int i = startY; i <= endY; i++) {
                    float linePos = i * SECT_CORE_RADIUS * 2;
                    DrawLineV(
                        {topLeft.x, linePos},      // Start from left of screen
                        {bottomRight.x, linePos},   // End at right of screen
                        Fade(LIGHTGRAY, 0.5f)      // Semi-transparent gray
                    );
                }

                // Draw all sects in the current colony
                for (const auto& sect : currentColony->GetSects()) {
                    sect->DrawInColonyView(sect->GetPosition(), camera.zoom);
                }
            }

            // End camera transformation
            EndMode2D();

            DrawText("Colony View", 10, 10, 20, BLACK);
            DrawText("Press S for Sect View", 10, 40, 20, GRAY);
            DrawText("Press P for Planet View", 10, 70, 20, GRAY);
            break;
        }



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


    // Draw UI elements (not affected by camera)
    DrawText(TextFormat("Zoom: %.2f", camera.zoom), 10, screenHeight - 15, 20, GRAY);
    DrawText("Double-click to select", 10, GetScreenHeight() - 30, 20, DARKGRAY);


    EndDrawing();
}

