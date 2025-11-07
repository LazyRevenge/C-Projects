#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "IntHashMap.h"
#include "DriverTable.h"
#include "CityTable.h"
#include "FineTable.h"

class FineRegistry {
private:
    // Узел списка нарушений
    struct ViolationNode {
        int    recordId;
        int    driverId;
        int    cityId;
        int    fineId;
        bool   paid;
        std::string date;
        ViolationNode* next;
        ViolationNode(int recordId, int driverId, int cityId,
            int fineId, bool paid, const std::string& date,
            ViolationNode* next)
            : recordId(recordId), driverId(driverId), cityId(cityId),
            fineId(fineId), paid(paid), date(date), next(next) {
        }
    };
public:
    struct ViolationInfo {
        int recordId;
        int driverId;
        int cityId;
        int fineId;
        bool paid;
        // Дополнительная информация
        std::string date;
        std::string driverName;
        std::string cityName;
        std::string fineType;
        double fineAmount;
        ViolationNode* next;

    };

    struct Filter {
        std::string field;
        int cmpType;
        std::string value;
        Filter* next = nullptr;
    };

private:

    // Основные поля
    ViolationNode* head;             // заголовочный узел
    IntHashMap recordToNodeMap;      // поиск по recordId
    mutable ViolationNode* currentIterator; // итератор

    // Ширины колонок (для форматированного вывода, не менялись)
    int recordIdWidth, driverIdWidth, cityIdWidth, fineIdWidth, paidWidth, dateWidth;

    // Вспомогательные методы
    void parseLine(const std::string& line);
    void addViolationNode(int recordId, int driverId, int cityId,
        int fineId, bool paid, const std::string& date);
    Filter* violationFilters = nullptr;
    bool matchFilter(const ViolationNode* node,
        const DriverTable& drivers,
        const CityTable& cities,
        const FineTable& fines) const;

public:
    // Конструктор / деструктор
    FineRegistry();
    ~FineRegistry();

    // Основные операции
    bool loadFromFile();
    bool loadFromFile(const std::string& filename);
    void saveToFile() const;
    void addViolation(int driverId, int cityId, int fineId, const std::string& date);
    void markAsPaid(int recordId);

    // Итератор по списку нарушений (возвращает подробную информацию)
    void violationIteratorReset() const;
    bool violationIteratorHasNext() const;
    ViolationInfo violationIteratorNext(const DriverTable& drivers,
        const CityTable& cities,
        const FineTable& fines) const;

    // Получить одно нарушение по recordId
    ViolationInfo getViolationById(int recordId,
        const DriverTable& drivers,
        const CityTable& cities,
        const FineTable& fines) const;
    
    // Работа с фильтром
    void addFilter(const std::string& field, int cmpType, const std::string& value);
    void clearFilters();
    void removeFilterAt(int index);
    int getFilterCount() const;
    std::string getFilterDescription(int index) const;

    // Обновление ссылок при удалении водителя или города
    void updateDriverReferences(int deletedDriverId);
    void updateCityReferences(int deletedCityId);

    // Обновление городов в уже существующих нарушениях (меняется driverId → cityId)
    void updateViolationsCity(int driverId, int newCityId);

    // ======== НОВЫЕ МЕТОДЫ ДЛЯ РЕДАКТИРОВАНИЯ НАРУШЕНИЯ ========
    bool updateViolationDriver(int recordId, int newDriverId, int newCityId);
    bool updateViolationFine(int recordId, int newFineId);
    bool updateViolationDate(int recordId, const std::string& newDate);
    bool updateViolationPaid(int recordId, bool paid);

    ViolationInfo getViolationInfo(
        const ViolationNode* node,         // Было ViolationMode* → ошибка!
        const DriverTable& drivers,
        const CityTable& cities,
        const FineTable& fines
    ) const;
    
    std::vector<ViolationInfo> applyFilters(const DriverTable& drivers,
        const CityTable& cities,
        const FineTable& fines) const;
};