#include "sect.h"
#include <iostream>

Sect::Sect() : core(nullptr), color(0), development_percentage(0.0f) {
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

void Sect::CalculateProduction() {
    // TODO: Implement production calculation based on all units
    std::cout << "Sect production calculated." << std::endl;
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
