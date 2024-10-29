#include "sect.h"
#include <iostream>

Sect::Sect()
    : core(nullptr),
      SectPosition({0, 0}),
      radius(0),
      color(BLUE),
      development_percentage(0.0f)
{
    CreateInitialUnits();
}

Sect::~Sect() {
    for (auto unit : units) {
        delete unit;
    }
}

void Sect::AddUnit(Unit* unit) {
    units.push_back(unit);
    std::cout << "New unit added to the sect." << std::endl;
}


void Sect::ConsumeResources() {
    // TODO: Implement resource consumption logic
    std::cout << "Sect resources consumed." << std::endl;
}

void Sect::BuildUnit(std::string unit_type) {
    // TODO: Implement unit building logic
    std::cout << "Building new unit of type: " << unit_type << std::endl;
}

void Sect::UpgradeUnit(Unit* unit) {
    // TODO: Implement unit upgrade logic
    std::cout << "Upgrading unit." << std::endl;
}

void Sect::Update() {
    // TODO: Implement sect update logic
    std::cout << "Sect updated." << std::endl;
}

void Sect::CreateInitialUnits() {
    std::vector<std::string> unit_types = {
        "Extraction", "Farming", "Energy", "Manufacture",
        "Construction", "Transport", "Research", "Commerce"
    };

    for (const auto& type : unit_types) {
        Unit* unit = new Unit(type);
        if (type == "Extraction") {
            unit->SetStatus("active");
            core = unit; // Set the Extraction unit as the core
        } else {
            unit->SetStatus("inactive");
        }
        AddUnit(unit);
    }

    std::cout << "All initial units created for the sect." << std::endl;
}

void Sect::DrawInColonyView(Vector2 pos, float scale) {
    radius = 50.0f * scale; // Scale the radius based on zoom level

    // Draw main sect circle (smaller in Colony view)
    DrawCircle(pos.x, pos.y, radius, color);
    DrawCircleLines(pos.x, pos.y, radius, BLACK);



    // Draw active units indicator as small dots around the sect
    float indicatorRadius = radius * 0.2f;

    for (int i = 0; i < units.size(); i++) {
        float angle = (i * 360.0f / units.size()) * DEG2RAD;
        Vector2 indicatorPos = {
            pos.x + (radius * 0.7f) * cosf(angle),
            pos.y + (radius * 0.7f) * sinf(angle)
        };
        units[i]->SetUnitPosInSectView(indicatorPos);
        units[i]->SetUnitRadiusInSectView(indicatorRadius);
        if ( units[i]->GetStatus() == "active" ) {DrawCircle(indicatorPos.x, indicatorPos.y, indicatorRadius, GREEN);}
        else {DrawCircle(indicatorPos.x, indicatorPos.y, indicatorRadius, GRAY);}
    }

    // Draw development percentage as a progress arc
    if (development_percentage > 0) {
        DrawRing(
            pos,
            radius * 1.1f,
            radius * 1.2f,
            0,
            development_percentage * 360,
            32,
            Fade(GREEN, 0.5f)
        );
    }
}

void Sect::DrawInSectView(Vector2 position) {
    float coreRadius = GetScreenHeight() * 0.3f;  // Core takes 60% of screen height

    // Draw the main core circle
    DrawCircle(position.x, position.y, coreRadius, LIGHTGRAY);
    DrawCircleLines(position.x, position.y, coreRadius, BLACK);

    // Draw core information
    DrawText(TextFormat("Development: %.1f%%", development_percentage * 100),
            position.x - MeasureText("Development: 100.0%", 20)/2,
            position.y - 10,
            20,
            BLACK);

    // Draw resource stats in the core
    DrawResourceStats(position, coreRadius);

    // Draw the units around the core
    float unitRadius = coreRadius * 0.2f;  // Units are 20% the size of core
    float orbitRadius = coreRadius * 1.4f; // Distance from core to units

    for (size_t i = 0; i < units.size(); ++i) {
        // Start from 90 degrees (top) and go clockwise
        float angle = (90.0f - (i * 45.0f)) * DEG2RAD;  // 8 units, 45 degrees apart

        Vector2 unitPos = {
            position.x + orbitRadius * cosf(angle),
            position.y - orbitRadius * sinf(angle)  // Subtract because Y grows downward
        };

        // Store the position for click detection
        units[i]->SetUnitPosInSectView(unitPos);
        units[i]->SetUnitRadiusInSectView(unitRadius);

        // Draw the unit circle
        Color fillColor = units[i]->GetStatus() == "active" ? GREEN : GRAY;
        DrawCircle(unitPos.x, unitPos.y, unitRadius, fillColor);
        DrawCircleLines(unitPos.x, unitPos.y, unitRadius, BLACK);

        // Draw first letter of unit type (only in Sect view)
        const char* unitType = units[i]->GetUnitType().c_str();
        char firstLetter[2] = {unitType[0], '\0'};

        // Center the letter in the circle
        int fontSize = (int)(unitRadius);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), firstLetter, fontSize, 1);
        Vector2 textPos = {
            unitPos.x - textSize.x/2,
            unitPos.y - textSize.y/2
        };

        DrawText(firstLetter, textPos.x, textPos.y, fontSize, BLACK);
    }

    // Draw the transparent right panel
    DrawTransparentRightPanel();
}

void Sect::DrawResourceStats(Vector2 position, float coreRadius) {
    // Draw production/consumption stats in the core
    const float statsY = position.y - coreRadius * 0.5f;
    const float statsSpacing = 25;
    int statIndex = 0;

    // Example resource stats (replace with actual resource tracking)
    std::vector<std::pair<const char*, int>> stats = {
        {"Energy: ", 100},
        {"Iron: ", 50},
        {"Food: ", 75},
    };

    for (const auto& stat : stats) {
        const char* text = TextFormat("%s%d", stat.first, stat.second);
        DrawText(text,
                position.x - MeasureText(text, 20)/2,
                statsY + statIndex * statsSpacing,
                20,
                BLACK);
        statIndex++;
    }
}


void Sect::DrawTransparentRightPanel() {
    int panelWidth = 100;
    Rectangle panel = {
        (float)GetScreenWidth() - panelWidth,
        0,
        (float)panelWidth,
        (float)GetScreenHeight()
    };
    DrawRectangleRec(panel, Fade(WHITE, 0.5f));

    // Draw panel content (e.g., notifications, alerts)
    DrawText("Updates",
            GetScreenWidth() - panelWidth + 10,
            10,
            20,
            BLACK);
}
