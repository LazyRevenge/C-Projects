// DatabaseManager.h
#pragma once

#include "CityTable.h"
#include "DriverTable.h"
#include "FineTable.h"
#include "FineRegistry.h"

#include <string>
#include <vector>

class DatabaseManager {
private:
    // Основные таблицы
    CityTable    cities;
    DriverTable  drivers;
    FineTable    fines;
    FineRegistry registry;

    // Внешние таблицы для слияния
    CityTable    externalCities;
    DriverTable  externalDrivers;
    FineTable    externalFines;
    FineRegistry externalRegistry;

public:
    // Загрузка/сохранение основной базы
    void loadAll();
    void saveAll();

    // Операции над основной базой
    void addCity(const std::string& name, int population,
        CityTable::PopulationGrade grade,
        CityTable::SettlementType type);
    void deleteCity(const std::string& name);

    void addDriver(const std::string& fullName,
        const std::string& birthDate,
        const std::string& cityName);
    void deleteDriverById(int driverId);

    void addFine(const std::string& type, double amount,
        FineTable::Severity severity = FineTable::Severity::LIGHT);
    void deleteFine(const std::string& type);

    void addViolation(const std::string& driverName,
        const std::string& fineType,
        const std::string& date);
    void markFineAsPaid(int recordId);

    std::vector<FineRegistry::ViolationInfo> getAllViolations();

    CityTable& getCities() { return cities; }
    DriverTable& getDrivers() { return drivers; }
    FineTable& getFines() { return fines; }
    FineRegistry& getRegistry() { return registry; }

    // Слияние внешней базы
    // suffix — суффикс в именах файлов, например "_ext"
    void loadExternalTables(const std::string& suffix);
    void mergeExternalTables();
    void saveMainTables();
};
