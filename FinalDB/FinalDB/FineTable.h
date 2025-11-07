#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "IntHashMap.h"

class FineTable {
public:
    enum class Severity { LIGHT, MEDIUM, HEAVY };
    struct FineInfo {
        int    id;
        double amount;
        std::string type;
        Severity severity;
    };

    FineTable();
    ~FineTable();

    bool loadFromFile();
    bool loadFromFile(const std::string& filename);
    void saveToFile() const;
    void addFine(const std::string& type, double amount,
        Severity severity = Severity::LIGHT);
    void deleteFine(const std::string& type);

    // Фильтры
    void addFilter(const std::string& field, int cmpType, const std::string& value);
    void clearFilters();
    FineInfo* applyFilters(int& outCount) const;

    int getFilterCount() const;
    std::string getFilterDescription(int index) const;
    void removeFilterAt(int index);

    void fineIteratorReset() const;
    bool fineIteratorHasNext() const;
    FineInfo fineIteratorNext() const;

    int getFineIdByType(const std::string& type) const;
    double getAmountById(int id) const;

    static std::string severityToString(Severity severity);
    std::string getFineTypeById(int id) const;

    bool updateFineType(int id, const std::string& newType);
    bool updateFineAmount(int id, double newAmount);
    bool updateFineSeverity(int id, Severity newSeverity);

private:
    struct FineNode {
        int    id;
        double amount;
        std::string type;
        Severity severity;
        FineNode* next;
        FineNode(int id, double amount, const std::string& type,
            Severity severity, FineNode* next)
            : id(id), amount(amount), type(type),
            severity(severity), next(next) {
        }
    };

    struct Filter {
        std::string field;
        int cmpType; // 1: contains, 2: equals, 3: <, 4: >
        std::string value;
        Filter* next;
    };

    FineNode* head;
    IntHashMap idToFineMap;
    std::map<std::string, int> typeToIdMap;
    mutable FineNode* currentIterator;

    Filter* currentFilter;

    int idWidth, amountWidth, typeWidth, severityWidth;

    void parseLine(const std::string& line);
    void addFineNode(int id, double amount, const std::string& type,
        Severity severity);
    bool matchField(const FineNode* node, const std::string& field,
        int cmpType, const std::string& value) const;
    FineInfo cloneInfo(const FineNode* node) const;
};