#ifndef SECT_H
#define SECT_H

#include "raylib.h"
#include <vector>
#include <utility>
#include <map>
#include "unit.h"
#include <cmath>  // Add this for cosf, sinf, etc.

class Sect {
public:
    Sect();
    ~Sect();

    void AddUnit(Unit* unit);
    void CalculateProduction();
    void ConsumeResources();
    void BuildUnit(std::string unit_type);
    void UpgradeUnit(Unit* unit);
    void Update();
    void Draw(Vector2 position);
    void DrawInColonyView(Vector2 position, float scale);
    void DrawInSectView(Vector2 position);

    // Setters
    void SetPosition(Vector2 position) {SectPosition = position;}

    // Getters
    Vector2 GetPosition() const {return SectPosition;}
    const std::vector<Unit*>& GetUnits() const { return units; }
    float GetRadius() const { return coreRadius; }

private:
    Vector2 SectPosition;
    float coreRadius;
    float defaultCoreRadius = 50.0f;
    std::vector<Unit*> units;
    Unit* core; // The Core Unit
    std::pair<int, int> location;
    Color color; // Unique color identifier
    std::vector<std::string> production_priority;
    std::map<std::string, int> resources;
    float development_percentage;

    void CreateInitialUnits();
    void DrawTransparentRightPanel();
    void DrawResourceStats(Vector2 position, float coreRadius);
};

#endif // SECT_H
