#ifndef SECT_H
#define SECT_H

#include <vector>
#include <utility>
#include <map>
#include "unit.h"

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

private:
    std::vector<Unit*> units;
    Unit* core; // The Core Unit
    std::pair<int, int> location;
    int color; // Unique color identifier
    std::vector<std::string> production_priority;
    std::map<std::string, int> resources;
    float development_percentage;

    void CreateInitialUnits();
};

#endif // SECT_H
