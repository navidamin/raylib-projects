#ifndef COLONY_H
#define COLONY_H

#include <vector>
#include <utility>
#include "sect.h"

class Colony {
public:
    Colony();
    ~Colony();

    void AddSect(Sect* sect);
    void BuildRoad(Sect* sect_a, Sect* sect_b);
    void ManageResources();
    void UnlockResearch();
    Vector2 CalculateCentroid();

private:
    std::vector<Sect*> sects;
    std::pair<int, int> centroid;
    int jurisdiction_radius;
    std::map<std::string, int> available_resources;
    std::vector<std::pair<Sect*, Sect*>> roads;
    int research_level;
    // Add transport_network when implemented
};

#endif // COLONY_H
