#include "CityTable.h"
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

CityTable::CityTable()
    : head(new CityNode(-1, "", 0, PopulationGrade::SMALL, SettlementType::CITY, nullptr)),
    currentFilter(nullptr),
    idWidth(5),
    nameWidth(20),
    populationWidth(12),
    typeWidth(10),
    currentIterator(nullptr)
{
    loadFromFile();
    updateColumnWidths();
}

CityTable::~CityTable() {
    CityNode* curr = head->next;
    while (curr) {
        CityNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
    delete head;

    Filter* f = currentFilter;
    while (f) {
        Filter* nxt = f->next;
        delete f;
        f = nxt;
    }
}

bool CityTable::loadFromFile() {
    return loadFromFile("cities.txt");
}

bool CityTable::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening cities file: " << filename << "\n";
        return false;
    }
    // Очистка
    CityNode* curr = head->next;
    while (curr) {
        CityNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head->next = nullptr;
    idToCityMap.clear();
    nameToIdMap.clear();

    std::string line;
    while (std::getline(file, line)) parseLine(line);
    file.close();
    updateColumnWidths();
    return true;
}

void CityTable::parseLine(const std::string& line) {
    if (line.empty()) return;
    istringstream iss(line);
    int id, population;
    string name, gradeStr, typeStr;
    iss >> id >> quoted(name) >> population >> quoted(gradeStr) >> quoted(typeStr);

    PopulationGrade grade = PopulationGrade::SMALL;
    if (gradeStr == "Medium")  grade = PopulationGrade::MEDIUM;
    else if (gradeStr == "Large") grade = PopulationGrade::LARGE;

    SettlementType type = SettlementType::CITY;
    if (typeStr == "Town")    type = SettlementType::TOWN;
    else if (typeStr == "Village") type = SettlementType::VILLAGE;

    addCityNode(id, name, population, grade, type);
}

void CityTable::addCityNode(int id, const std::string& name, int population,
    PopulationGrade grade, SettlementType type)
{
    CityNode* newNode = new CityNode(id, name, population, grade, type, head->next);
    head->next = newNode;
    idToCityMap.insert(id, newNode);
    nameToIdMap[name] = id;
}

void CityTable::addCity(const std::string& name, int population,
    PopulationGrade grade, SettlementType type)
{
    int newId = 1;
    for (auto& pair : nameToIdMap) {
        if (pair.second >= newId) newId = pair.second + 1;
    }
    addCityNode(newId, name, population, grade, type);
    updateColumnWidths();
}

void CityTable::deleteCity(const std::string& name) {
    auto it = nameToIdMap.find(name);
    if (it == nameToIdMap.end()) return;
    int id = it->second;
    CityNode* prev = head;
    CityNode* curr = head->next;
    while (curr) {
        if (curr->id == id) {
            prev->next = curr->next;
            idToCityMap.remove(id);
            nameToIdMap.erase(name);
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void CityTable::saveToFile() const {
    ofstream file("cities.txt");
    if (!file.is_open()) {
        cerr << "Error opening cities file for writing!" << endl;
        return;
    }
    CityNode* curr = head->next;
    while (curr) {
        file << curr->id << ' '
            << quoted(curr->name) << ' '
            << curr->population << ' '
            << quoted(populationGradeToString(curr->grade)) << ' '
            << quoted(settlementTypeToString(curr->type))
            << '\n';
        curr = curr->next;
    }
    file.close();
}

void CityTable::updateColumnWidths() {
    CityNode* curr = head->next;
    int maxIdLen = 1;
    size_t maxNameLen = 0;
    while (curr) {
        int idLen = static_cast<int>(to_string(curr->id).length());
        if (idLen > maxIdLen) maxIdLen = idLen;
        if (curr->name.length() > maxNameLen) maxNameLen = curr->name.length();
        curr = curr->next;
    }
    idWidth = maxIdLen + 2;
    nameWidth = static_cast<int>(maxNameLen) + 4;
}

std::string CityTable::formatNode(const CityNode* node) const {
    ostringstream oss;
    oss << left
        << setw(nameWidth) << node->name
        << setw(populationWidth) << node->population
        << setw(typeWidth) << settlementTypeToString(node->type)
        << setw(typeWidth) << populationGradeToString(node->grade);
    return oss.str();
}

void CityTable::addFilter(const std::string& field, int cmpType, const std::string& value) {
    Filter* newFilter = new Filter{ field, cmpType, value, nullptr };
    if (!currentFilter) {
        currentFilter = newFilter;
    }
    else {
        Filter* temp = currentFilter;
        while (temp->next) temp = temp->next;
        temp->next = newFilter;
    }
}

void CityTable::clearFilters() {
    Filter* f = currentFilter;
    while (f) {
        Filter* nxt = f->next;
        delete f;
        f = nxt;
    }
    currentFilter = nullptr;
}

CityTable::CityNode* CityTable::applyFilters() const {
    CityNode* filteredDummy = new CityNode(-1, "", 0, PopulationGrade::SMALL, SettlementType::CITY, nullptr);
    CityNode* tail = filteredDummy;
    CityNode* curr = head->next;
    while (curr) {
        bool match = true;
        Filter* f = currentFilter;
        while (f) {
            if (!matchField(curr, f->field, f->cmpType, f->value)) {
                match = false;
                break;
            }
            f = f->next;
        }
        if (match) {
            CityNode* newNode = cloneNode(curr);
            tail->next = newNode;
            tail = newNode;
        }
        curr = curr->next;
    }
    CityNode* result = filteredDummy->next;
    delete filteredDummy;
    return result;
}

bool CityTable::matchField(const CityNode* node, const std::string& field,
    int cmpType, const std::string& value) const
{
    if (field == "name") {
        if (cmpType == 1) { // contains
            return node->name.find(value) != string::npos;
        }
        else if (cmpType == 2) { // equals
            return node->name == value;
        }
    }
    else if (field == "population") {
        return checkNumeric(node->population, cmpType, value);
    }
    else if (field == "type") {
        string typeStr = settlementTypeToString(node->type);
        if (cmpType == 2) {
            return typeStr == value;
        }
    }
    else if (field == "grade") {
        string gradeStr = populationGradeToString(node->grade);
        if (cmpType == 2) {
            return gradeStr == value;
        }
    }
    return false;
}

bool CityTable::checkNumeric(int value, int cmpType, const std::string& valueStr) const {
    int v = stoi(valueStr);
    if (cmpType == 3) return value < v;
    if (cmpType == 4) return value > v;
    return value == v;
}

CityTable::CityNode* CityTable::cloneNode(const CityNode* src) const {
    return new CityNode(src->id, src->name, src->population, src->grade, src->type, nullptr);
}

std::string CityTable::populationGradeToString(PopulationGrade grade) {
    switch (grade) {
    case PopulationGrade::SMALL:  return "Small";
    case PopulationGrade::MEDIUM: return "Medium";
    case PopulationGrade::LARGE:  return "Large";
    default:                      return "Small";
    }
}

std::string CityTable::settlementTypeToString(SettlementType type) {
    switch (type) {
    case SettlementType::CITY:    return "City";
    case SettlementType::TOWN:    return "Town";
    case SettlementType::VILLAGE: return "Village";
    default:                      return "Small";
    }
}

void CityTable::cityIteratorReset(CityNode* start) const {
    currentIterator = start ? start : head->next;
}

bool CityTable::cityIteratorHasNext() const {
    return currentIterator != nullptr;
}

CityTable::CityInfo CityTable::cityIteratorNext() const {
    CityInfo info{};
    if (!currentIterator) return info;
    info.id = currentIterator->id;
    info.name = currentIterator->name;
    info.population = currentIterator->population;
    info.grade = currentIterator->grade;
    info.type = currentIterator->type;
    currentIterator = currentIterator->next;
    return info;
}

bool CityTable::cityExists(int cityId) const {
    return idToCityMap.find<CityNode>(cityId) != nullptr;
}

std::string CityTable::getCityNameById(int id) const {
    CityNode* node = idToCityMap.find<CityNode>(id); //Возвращает название города по ID
    return node ? node->name : "";
}

int CityTable::getCityIdByName(const std::string& name) const {
    auto it = nameToIdMap.find(name); //Возвращает ID города по его названию
    return (it != nameToIdMap.end()) ? it->second : -1;
}

bool CityTable::updateCityName(int id, const std::string& newName) { //Обновляет название города по ID.
    CityNode* node = idToCityMap.find<CityNode>(id);
    if (!node) return false;
    nameToIdMap.erase(node->name);
    node->name = newName;
    nameToIdMap[newName] = id;
    updateColumnWidths();
    return true;
}

bool CityTable::updateCityPopulation(int id, int newPopulation) {
    CityNode* node = idToCityMap.find<CityNode>(id);
    if (!node) return false;
    node->population = newPopulation;
    return true;
}

bool CityTable::updateCityGrade(int id, PopulationGrade newGrade) {
    CityNode* node = idToCityMap.find<CityNode>(id);
    if (!node) return false;
    node->grade = newGrade;
    return true;
}

bool CityTable::updateCityType(int id, SettlementType newType) {
    CityNode* node = idToCityMap.find<CityNode>(id);
    if (!node) return false;
    node->type = newType;
    return true;
}

int CityTable::getFilterCount() const {
    int count = 0;
    Filter* f = currentFilter;
    while (f) {
        count++;
        f = f->next;
    }
    return count;
}

std::string CityTable::getFilterDescription(int index) const {
    int i = 0;
    Filter* f = currentFilter;
    while (f) {
        if (i == index) {
            ostringstream oss;
            if (f->field == "name") {
                if (f->cmpType == 1) oss << "Name contains \"" << f->value << "\"";
                else if (f->cmpType == 2) oss << "Name equals \"" << f->value << "\"";
            }
            else if (f->field == "population") {
                if (f->cmpType == 3) oss << "Population < " << f->value;
                else if (f->cmpType == 4) oss << "Population > " << f->value;
                else if (f->cmpType == 2) oss << "Population = " << f->value;
            }
            else if (f->field == "type") {
                oss << "Type equals \"" << f->value << "\"";
            }
            else if (f->field == "grade") {
                oss << "Grade equals \"" << f->value << "\"";
            }
            return oss.str();
        }
        f = f->next;
        i++;
    }
    return "";
}

void CityTable::removeFilterAt(int index) {
    if (!currentFilter) return;
    if (index == 0) {
        Filter* toDelete = currentFilter;
        currentFilter = toDelete->next;
        delete toDelete;
        return;
    }
    int i = 0;
    Filter* prev = currentFilter;
    while (prev && prev->next) {
        if (i + 1 == index) {
            Filter* toDelete = prev->next;
            prev->next = toDelete->next;
            delete toDelete;
            return;
        }
        prev = prev->next;
        i++;
    }
}