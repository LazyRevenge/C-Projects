#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <regex>
#include <map>
#include "IntHashMap.h"
#include <ctime>
#include <vector>

class DriverTable {
public:
    // Структура для передачи информации о водителе
    struct DriverInfo {
        int    id;
        std::string fullName;
        std::string birthDate;
        int    cityId;
    };

    // Конструктор / деструктор
    DriverTable();
    ~DriverTable();

    // Основные операции
    bool loadFromFile();
    bool loadFromFile(const std::string& filename);
    void saveToFile() const;
    void addDriver(const std::string& fullName,
        const std::string& birthDate,
        int cityId);
    void deleteDriverById(int id);

    // Итератор по списку водителей
    void driverIteratorReset() const;
    bool driverIteratorHasNext() const;
    DriverInfo driverIteratorNext() const;

    // Геттеры
    int getDriverId(const std::string& fullName,
        const std::string& birthDate = "",
        int cityId = -1) const;
    int getCityIdForDriver(const std::string& fullName) const;
    std::string getDriverNameById(int id) const;

    // Обновление ссылок при удалении города
    void updateCityReferences(int deletedCityId);

    // Редактирование данных водителя
    bool updateDriverName(int id, const std::string& newName);
    bool updateDriverBirthDate(int id, const std::string& newBirthDate);
    bool updateDriverCity(int id, int newCityId);

    // Фильтры
    void addFilter(const std::string& field, int cmpType, const std::string& value);
    void clearFilters();
    DriverInfo* applyFilters(int& outCount) const;

    int getFilterCount() const;
    std::string getFilterDescription(int index) const;
    void removeFilterAt(int index);

    // Вспомогательное: вернуть всех водителей с данным ФИО
    std::vector<DriverInfo> findAllByName(const std::string& fullName) const;

private:
    // Узел списка водителей
    struct DriverNode {
        int    id;
        std::string fullName;
        std::string birthDate;
        int    cityId;
        DriverNode* next;
        DriverNode(int id, const std::string& fullName, const std::string& birthDate,
            int cityId, DriverNode* next)
            : id(id), fullName(fullName), birthDate(birthDate),
            cityId(cityId), next(next) {
        }
    };

    // Структура фильтра
    struct Filter {
        std::string field;
        int cmpType; // 1: contains, 2: equals
        std::string value;
        Filter* next;
    };

    DriverNode* head;                 // заголовочный узел
    IntHashMap idToDriverMap;         // поиск по ID
    std::map<std::string, int> nameToIdMap; // ФИО → ID (хранит только последнее вхождение)
    mutable DriverNode* currentIterator;
    Filter* currentFilter;

    int idWidth, nameWidth, birthDateWidth, cityIdWidth;

    void parseLine(const std::string& line);
    void addDriverNode(int id, const std::string& fullName,
        const std::string& birthDate, int cityId);

    bool validateName(const std::string& name) const;
    bool validateDate(const std::string& date) const;
    bool validateAge(const std::string& birthDate) const;

    bool matchField(const DriverNode* node, const std::string& field,
        int cmpType, const std::string& value) const;
    DriverInfo cloneInfo(const DriverNode* node) const;
};