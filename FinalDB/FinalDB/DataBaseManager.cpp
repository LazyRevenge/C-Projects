// DatabaseManager.cpp
#include "DatabaseManager.h"
#include <iostream>
#include <stdexcept>

void DatabaseManager::loadAll() {
    cities.loadFromFile();
    drivers.loadFromFile();
    fines.loadFromFile();
    registry.loadFromFile();
}

void DatabaseManager::saveAll() {
    cities.saveToFile();
    drivers.saveToFile();
    fines.saveToFile();
    registry.saveToFile();
}

void DatabaseManager::addCity(const std::string& name,
    int population,
    CityTable::PopulationGrade grade,
    CityTable::SettlementType type)
{
    cities.addCity(name, population, grade, type);
    cities.saveToFile();
}

void DatabaseManager::deleteCity(const std::string& name) {
    int id = cities.getCityIdByName(name);
    if (id != -1) {
        cities.deleteCity(name);
        drivers.updateCityReferences(id);
        registry.updateCityReferences(id);
        saveAll();
    }
}

void DatabaseManager::addDriver(const std::string& fullName,
    const std::string& birthDate,
    const std::string& cityName)
{
    int cityId = cities.getCityIdByName(cityName);
    if (cityId == -1) {
        throw std::invalid_argument("City does not exist");
    }
    drivers.addDriver(fullName, birthDate, cityId);
    saveAll();
}

void DatabaseManager::deleteDriverById(int driverId) {
    drivers.deleteDriverById(driverId);
    registry.updateDriverReferences(driverId);
    saveAll();
}

void DatabaseManager::addFine(const std::string& type,
    double amount,
    FineTable::Severity severity)
{
    fines.addFine(type, amount, severity);
    saveAll();
}

void DatabaseManager::deleteFine(const std::string& type) {
    fines.deleteFine(type);
    saveAll();
}

void DatabaseManager::addViolation(const std::string& driverName,
    const std::string& fineType,
    const std::string& date)
{
    int driverId = drivers.getDriverId(driverName);
    int cityId = drivers.getCityIdForDriver(driverName);
    int fineId = fines.getFineIdByType(fineType);
    if (driverId == -1 || cityId == -1 || fineId == -1) {
        throw std::invalid_argument("Invalid data for violation");
    }
    registry.addViolation(driverId, cityId, fineId, date);
    saveAll();
}

void DatabaseManager::markFineAsPaid(int recordId) {
    registry.markAsPaid(recordId);
    saveAll();
}

std::vector<FineRegistry::ViolationInfo> DatabaseManager::getAllViolations() {
    registry.violationIteratorReset();
    std::vector<FineRegistry::ViolationInfo> list;
    while (registry.violationIteratorHasNext()) {
        list.push_back(registry.violationIteratorNext(drivers, cities, fines));
    }
    return list;
}

void DatabaseManager::loadExternalTables(const std::string& suffix) {
    // 1) Сначала основная база (если нужно)
    cities.loadFromFile();
    drivers.loadFromFile();
    fines.loadFromFile();
    registry.loadFromFile();

    // 2) Читаем внешние файлы, без присваиваний объектов:
    externalCities.loadFromFile("cities" + suffix + ".txt");
    externalDrivers.loadFromFile("drivers" + suffix + ".txt");
    externalFines.loadFromFile("fines" + suffix + ".txt");
    externalRegistry.loadFromFile("registry" + suffix + ".txt");
}

void DatabaseManager::mergeExternalTables() {
    // 1) Города
    externalCities.cityIteratorReset();
    while (externalCities.cityIteratorHasNext()) {
        auto ext = externalCities.cityIteratorNext();
        int id = cities.getCityIdByName(ext.name);
        if (id == -1) {
            cities.addCity(ext.name, ext.population, ext.grade, ext.type);
        }
        else {
            cities.updateCityPopulation(id, ext.population);
            cities.updateCityGrade(id, ext.grade);
            cities.updateCityType(id, ext.type);
        }
    }

    // 2) Водители
    externalDrivers.driverIteratorReset();
    while (externalDrivers.driverIteratorHasNext()) {
        auto ext = externalDrivers.driverIteratorNext();
        // Находим/создаем город в основной базе
        std::string cityName = externalCities.getCityNameById(ext.cityId);
        int mainCityId = cities.getCityIdByName(cityName);
        if (mainCityId == -1) {
            cities.addCity(cityName, 0,
                CityTable::PopulationGrade::SMALL,
                CityTable::SettlementType::CITY);
            mainCityId = cities.getCityIdByName(cityName);
        }
        int did = drivers.getDriverId(ext.fullName, ext.birthDate, mainCityId);
        if (did == -1) {
            drivers.addDriver(ext.fullName, ext.birthDate, mainCityId);
        }
        else {
            drivers.updateDriverName(did, ext.fullName);
            drivers.updateDriverBirthDate(did, ext.birthDate);
            drivers.updateDriverCity(did, mainCityId);
        }
    }

    // 3) Штрафы
    externalFines.fineIteratorReset();
    while (externalFines.fineIteratorHasNext()) {
        auto ext = externalFines.fineIteratorNext();
        int fid = fines.getFineIdByType(ext.type);
        if (fid == -1) {
            fines.addFine(ext.type, ext.amount, ext.severity);
        }
        else {
            fines.updateFineAmount(fid, ext.amount);
            fines.updateFineSeverity(fid, ext.severity);
        }
    }

    // 4) Нарушения
    auto extList = externalRegistry.applyFilters(externalDrivers, externalCities, externalFines);
    for (auto& v : extList) {
        bool found = false;
        registry.violationIteratorReset();
        while (registry.violationIteratorHasNext()) {
            auto mv = registry.violationIteratorNext(drivers, cities, fines);
            if (mv.driverName == v.driverName
                && mv.cityName == v.cityName
                && mv.fineType == v.fineType
                && mv.date == v.date)
            {
                registry.updateViolationPaid(mv.recordId, v.paid);
                registry.updateViolationFine(mv.recordId, v.fineId);
                found = true;
                break;
            }
        }
        if (!found) {
            // 1) Найдём дату рождения водителя во внешней таблице
            externalDrivers.driverIteratorReset();
            std::string birthDate;
            while (externalDrivers.driverIteratorHasNext()) {
                auto di = externalDrivers.driverIteratorNext();
                if (di.id == v.driverId) {
                    birthDate = di.birthDate;
                    break;
                }
            }

            // 2) Определяем mainCityId
            int mainCityId = cities.getCityIdByName(v.cityName);

            // 3) Ищем в основной таблице водителя по ФИО+birthDate+город
            int driverId = drivers.getDriverId(v.driverName, birthDate, mainCityId);

            // 4) Добавляем нарушение
            int fineId = fines.getFineIdByType(v.fineType);
            registry.addViolation(driverId, mainCityId, fineId, v.date);

            // 5) Если нужно, помечаем оплачено
            if (v.paid) {
                int maxId = 0;
                registry.violationIteratorReset();
                while (registry.violationIteratorHasNext()) {
                    auto info = registry.violationIteratorNext(drivers, cities, fines);
                    maxId = std::max(maxId, info.recordId);
                }
                registry.markAsPaid(maxId);
            }
        }
    }

    // Сохраняем объединённую базу сразу
    saveAll();
}

void DatabaseManager::saveMainTables() {
    saveAll();
}
