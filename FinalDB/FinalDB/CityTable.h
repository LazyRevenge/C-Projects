#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include "IntHashMap.h"
#include <map>

class CityTable {
public:
    struct CityNode {
        int    id;
        std::string name;
        int    population;
        enum class PopulationGrade { SMALL, MEDIUM, LARGE };
        enum class SettlementType { CITY, TOWN, VILLAGE };
        PopulationGrade grade;
        SettlementType  type;
        CityNode* next;
        CityNode(int id, const std::string& name, int population,
            PopulationGrade grade, SettlementType type, CityNode* next)
            : id(id), name(name), population(population),
            grade(grade), type(type), next(next) {
        }
    };

    using PopulationGrade = CityNode::PopulationGrade;
    using SettlementType = CityNode::SettlementType;

    struct CityInfo {
        int    id;
        std::string name;
        int    population;
        PopulationGrade grade;
        SettlementType  type;
    };

    CityTable();
    ~CityTable();

    /// Загрузить из default-файла cities.txt
    bool loadFromFile();
    /// Загрузить из произвольного файла
    bool loadFromFile(const std::string& filename);
    void saveToFile() const;
    void addCity(const std::string& name, int population,
        PopulationGrade grade, SettlementType type);
    void deleteCity(const std::string& name);

    // Фильтры
    void addFilter(const std::string& field, int cmpType, const std::string& value);
    void clearFilters();
    CityNode* applyFilters() const;

    int getFilterCount() const;
    std::string getFilterDescription(int index) const;
    void removeFilterAt(int index);

    static std::string populationGradeToString(PopulationGrade grade);
    static std::string settlementTypeToString(SettlementType type);

    void updateColumnWidths();
    std::string formatNode(const CityNode* node) const;

    void cityIteratorReset(CityNode* start = nullptr) const;
    bool cityIteratorHasNext() const;
    CityInfo cityIteratorNext() const;

    bool cityExists(int cityId) const;
    std::string getCityNameById(int id) const;
    int         getCityIdByName(const std::string& name) const;

    bool updateCityName(int id, const std::string& newName);
    bool updateCityPopulation(int id, int newPopulation);
    bool updateCityGrade(int id, PopulationGrade newGrade);
    bool updateCityType(int id, SettlementType newType);

private:
    struct Filter {
        std::string field;
        int cmpType; // 1: contains, 2: equals, 3: <, 4: >
        std::string value;
        Filter* next;
    };

    CityNode* head;
    IntHashMap idToCityMap;
    std::map<std::string, int> nameToIdMap;
    Filter* currentFilter;
    mutable CityNode* currentIterator;

    int idWidth, nameWidth, populationWidth, typeWidth;

    void parseLine(const std::string& line);
    void addCityNode(int id, const std::string& name, int population,
        PopulationGrade grade, SettlementType type);
    bool matchField(const CityNode* node, const std::string& field,
        int cmpType, const std::string& value) const;
    bool checkNumeric(int value, int cmpType, const std::string& valueStr) const;
    CityNode* cloneNode(const CityNode* src) const;
};