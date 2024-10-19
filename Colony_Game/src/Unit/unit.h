#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <map>
#include <vector>

class Unit {
public:
    Unit(std::string type);
    ~Unit();

    void Start();
    void Stop();
    void Upgrade(int level);
    std::map<std::string, float> CalculateConsumption() const;
    std::map<std::string, float> CalculateProduction() const;
    void DisplayStats() const;
    void Update();
    void SetInitialParameters();

    std::string GetStatus() const { return status; }
    void SetStatus(const std::string& newStatus) { status = newStatus; }

private:
    std::string unit_type;
    std::map<std::string, float> parameters;
    std::map<std::string, float> consumption;
    std::map<std::string, float> production;
    std::string status;
    std::vector<std::string> upgrades;
    float energy_cost;
};

#endif // UNIT_H
