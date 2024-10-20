#include "colony.h"
#include <iostream>

Colony::Colony() : jurisdiction_radius(3), research_level(0) {
    // Initialize other members as needed
}

Colony::~Colony() {
    for (auto sect : sects) {
        delete sect;
    }
}

void Colony::AddSect(Sect* sect) {
    sects.push_back(sect);
    std::cout << "New sect added to the colony." << std::endl;
    CalculateCentroid();
}

void Colony::BuildRoad(Sect* sect_a, Sect* sect_b) {
    roads.push_back(std::make_pair(sect_a, sect_b));
    std::cout << "New road built between sects." << std::endl;
}

void Colony::ManageResources() {
    // TODO: Implement resource management logic
    std::cout << "Colony resources managed." << std::endl;
}

void Colony::UnlockResearch() {
    research_level++;
    std::cout << "Colony research level increased to " << research_level << std::endl;
    // TODO: Implement unlocking of new technologies based on research level
}

Vector2 Colony::CalculateCentroid() {
    // Implement centroid calculation logic here
    // For now, we'll just return a dummy value
    return { 0.0f, 0.0f };
}
