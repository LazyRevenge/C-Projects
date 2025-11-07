#include "UserInterface.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>
#include <clocale>
#include <iomanip>
#include <vector>
using namespace std;

// ======= Вспомогательные методы для работы с датами =======
bool UserInterface::parseDate(const std::string& dateStr, Date& date) {
    if (dateStr.size() != 10 || dateStr[2] != '.' || dateStr[5] != '.') return false;
    try {
        date.day = stoi(dateStr.substr(0, 2));
        date.month = stoi(dateStr.substr(3, 2));
        date.year = stoi(dateStr.substr(6, 4));
    }
    catch (...) {
        return false;
    }
    return true;
}

bool UserInterface::isDateValid(const Date& date) {
    if (date.year < 1900 || date.year > 2100) return false;
    if (date.month < 1 || date.month > 12) return false;
    if (date.day < 1 || date.day > 31) return false;
    static const int mdays[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    int maxDay = mdays[date.month];
    if (date.month == 2) {
        if ((date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0)) {
            maxDay = 29;
        }
    }
    return date.day <= maxDay;
}

int UserInterface::calculateAge(const Date& birthDate, const Date& violationDate) {
    int age = violationDate.year - birthDate.year;
    if (violationDate.month < birthDate.month ||
        (violationDate.month == birthDate.month && violationDate.day < birthDate.day)) {
        age--;
    }
    return age;
}

// ======= Сортировка статистики по городам =======
int UserInterface::compareCityStats(const void* a, const void* b) {
    const CityViolations* ca = static_cast<const CityViolations*>(a);
    const CityViolations* cb = static_cast<const CityViolations*>(b);
    return cb->count - ca->count;
}

void UserInterface::printCityViolations(CityViolations* stats, int cityCount) {
    std::cout << "+----------------------+-------------+------------+------------+\n";
    std::cout << "| City                 | Violations  | Date       | Paid       |\n";
    std::cout << "+----------------------+-------------+------------+------------+\n";
    for (int i = 0; i < cityCount; ++i) {
        for (int j = 0; j < stats[i].count; ++j) {
            auto vi = dbManager.getRegistry().getViolationById(
                stats[i].violationIds[j],
                dbManager.getDrivers(),
                dbManager.getCities(),
                dbManager.getFines()
            );
            ostringstream oss;
            oss << "| " << left << setw(20) << stats[i].name << " | "
                << right << setw(10) << stats[i].count << " | "
                << left << setw(10) << vi.date << " | "
                << left << setw(10) << (vi.paid ? "Yes" : "No") << " |";
            std::cout << oss.str() << "\n";
        }
    }
    std::cout << "+----------------------+-------------+------------+------------+\n";
}

void UserInterface::showTopDrivers() {
    std::cout << "\nTop-5 drivers by violation count:\n";
    auto violations = dbManager.getAllViolations();
    map<std::string, int> countMap;
    for (auto& v : violations) {
        countMap[v.driverName]++;
    }
    vector<pair<std::string, int>> vec(countMap.begin(), countMap.end());
    sort(vec.begin(), vec.end(), [](auto& a, auto& b) { return a.second > b.second; });
    for (size_t i = 0; i < vec.size() && i < 5; ++i) {
        std::cout << vec[i].first << ": " << vec[i].second << "\n";
    }
}

void UserInterface::mergeDatabaseMenu() {
    std::cout << "\n--- Merge External Database ---\n";
    std::string suf = readString("Enter suffix (e.g. _ext): ");
    try {
        dbManager.loadExternalTables(suf);
        dbManager.mergeExternalTables();
        std::cout << "Merge completed and saved.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Error during merge: " << e.what() << "\n";
    }
}


// ======= Глобальные меню =======
void UserInterface::run() {
    setlocale(LC_ALL, "");
    dbManager.loadAll();
    while (true) {
        mainMenu();
    }
}

void UserInterface::mainMenu() {
    dbManager.loadAll();
    while (true) {
        std::cout << "\n=== Main Menu ===\n";
        std::cout << "1. Manage Cities\n";
        std::cout << "2. Manage Drivers\n";
        std::cout << "3. Manage Fines\n";
        std::cout << "4. Manage Violations\n";
        std::cout << "5. Statistics\n";
        std::cout << "6. Merge External Database\n";   // <-- новый пункт
        std::cout << "7. Exit\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: citiesMenu();   break;
        case 2: driversMenu();  break;
        case 3: finesMenu();    break;
        case 4: registryMenu(); break;
        case 5: statisticsMenu(); break;
        case 6: mergeDatabaseMenu(); break;   // <-- обработка
        case 7: dbManager.saveAll(); exit(0);
        default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}


// ==================== Cities ====================
void UserInterface::citiesMenu() {
    while(true){
        std::cout << "\n--- Cities ---\n";
        std::cout << "1. List Cities\n";
        std::cout << "2. Add City\n";
        std::cout << "3. Delete City\n";
        std::cout << "4. Filter Cities\n";
        std::cout << "5. Remove Specific City Filters\n";
        std::cout << "6. Clear All City Filters\n";
        std::cout << "7. Edit City\n";
        std::cout << "8. Back\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: listCities();            break;
        case 2: addCity();               break;
        case 3: deleteCity();            break;
        case 4: filterCities();          break;
        case 5: removeCityFilters();     break;
        case 6: clearAllCityFilters();   break;
        case 7: editCity();              break;
        case 8: return;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::listCities() {
    CityTable& cities = dbManager.getCities();
    CityTable::CityNode* filteredHead = cities.applyFilters();
    cities.cityIteratorReset(filteredHead);

    std::cout << "+----------------------+------------+------------+------------+\n";
    std::cout << "| Name                 | Population | Type       | Grade      |\n";
    std::cout << "+----------------------+------------+------------+------------+\n";
    while (cities.cityIteratorHasNext()) {
        auto ci = cities.cityIteratorNext();
        ostringstream oss;
        oss << "| " << left << setw(20) << ci.name << " | "
            << right << setw(10) << ci.population << " | "
            << left << setw(10) << CityTable::settlementTypeToString(ci.type) << " | "
            << left << setw(10) << CityTable::populationGradeToString(ci.grade) << " |";
        std::cout << oss.str() << "\n";
    }
    std::cout << "+----------------------+------------+------------+------------+\n";

    CityTable::CityNode* tmp = filteredHead;
    while (tmp) {
        CityTable::CityNode* nxt = tmp->next;
        delete tmp;
        tmp = nxt;
    }
}

void UserInterface::addCity() {
    string name = readString("City name: ");
    int population = readInt("Population: ");
    std::cout << "Population grade (0-Small, 1-Medium, 2-Large): ";
    int gradeInput = readInt("");
    auto grade = static_cast<CityTable::PopulationGrade>(gradeInput);

    std::cout << "Settlement type (0-City, 1-Town, 2-Village): ";
    int typeInput = readInt("");
    auto type = static_cast<CityTable::SettlementType>(typeInput);

    dbManager.addCity(name, population, grade, type);
    std::cout << "City added.\n";
}

void UserInterface::deleteCity() {
    string name = readString("City name to delete: ");
    dbManager.deleteCity(name);
    std::cout << "City deleted (if it existed).\n";
}

void UserInterface::filterCities() {
    CityTable& cities = dbManager.getCities();
    std::cout << "\nFilter by:\n";
    std::cout << "1. Name contains\n";
    std::cout << "2. Name equals\n";
    std::cout << "3. Population <\n";
    std::cout << "4. Population >\n";
    std::cout << "5. Population equals\n";
    std::cout << "6. Type equals\n";
    std::cout << "7. Grade equals\n";
    std::cout << "8. Cancel\n";
    int choice = readInt("Choose filter type: ");
    if (choice == 8) return;
    if (choice == 1) {
        string val = readString("Enter substring: ");
        cities.addFilter("name", 1, val);
    }
    else if (choice == 2) {
        string val = readString("Enter full name: ");
        cities.addFilter("name", 2, val);
    }
    else if (choice == 3) {
        string val = readString("Enter threshold: ");
        cities.addFilter("population", 3, val);
    }
    else if (choice == 4) {
        string val = readString("Enter threshold: ");
        cities.addFilter("population", 4, val);
    }
    else if (choice == 5) {
        string val = readString("Enter exact value: ");
        cities.addFilter("population", 2, val);
    }
    else if (choice == 6) {
        std::cout << "Type options: City, Town, Village\n";
        string val = readString("Enter type: ");
        cities.addFilter("type", 2, val);
    }
    else if (choice == 7){
        std::cout << "Grade options: Small, Medium, Large\n";
        string val = readString("Enter grade: ");
        cities.addFilter("grade", 2, val);
    }
    else {
        std::cout << "Invalid option.\n";
        return;
    }
    std::cout << "Filter added.\n";
}

void UserInterface::removeCityFilters() {
    CityTable& cities = dbManager.getCities();
    while (true) {
        int count = cities.getFilterCount();
        if (count == 0) {
            std::cout << "No active filters to remove.\n";
            return;
        }
        std::cout << "\nActive Filters:\n";
        for (int i = 0; i < count; ++i) {
            std::cout << i + 1 << ". " << cities.getFilterDescription(i) << "\n";
        }
        std::cout << "0. Remove ALL filters\n";
        std::cout << "-1. Done removing\n";
        int choice = readInt("Choose option: ");
        if (choice == -1) {
            break;
        }
        else if (choice == 0) {
            cities.clearFilters();
            std::cout << "All filters removed.\n";
            break;
        }
        else if (choice >= 1 && choice <= count) {
            cities.removeFilterAt(choice - 1);
            std::cout << "Filter removed.\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::clearAllCityFilters() {
    dbManager.getCities().clearFilters();
    std::cout << "All city filters cleared.\n";
}

void UserInterface::editCity() {
    listCities();
    string name = readString("Enter City name to edit: ");
    CityTable& cities = dbManager.getCities();
    int id = cities.getCityIdByName(name);
    if (id == -1) {
        std::cout << "City not found.\n";
        return;
    }
    std::cout << "\nEdit field:\n";
    std::cout << "1. Name\n";
    std::cout << "2. Population\n";
    std::cout << "3. Grade\n";
    std::cout << "4. Type\n";
    std::cout << "5. Cancel\n";
    int choice = readInt("Choose field: ");
    if (choice == 1) {
        string newName = readString("Enter new name: ");
        if (cities.updateCityName(id, newName))
            std::cout << "Name updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 2) {
        int newPop = readInt("Enter new population: ");
        if (cities.updateCityPopulation(id, newPop))
            std::cout << "Population updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 3) {
        std::cout << "Grades: 0-Small, 1-Medium, 2-Large\n";
        int g = readInt("Choose grade: ");
        if (cities.updateCityGrade(id, static_cast<CityTable::PopulationGrade>(g)))
            std::cout << "Grade updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 4) {
        std::cout << "Types: 0-City, 1-Town, 2-Village\n";
        int t = readInt("Choose type: ");
        if (cities.updateCityType(id, static_cast<CityTable::SettlementType>(t)))
            std::cout << "Type updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else {
        std::cout << "Cancel.\n";
    }
}

// ==================== Drivers ====================
void UserInterface::driversMenu() {
    while(true){
        std::cout << "\n--- Drivers ---\n";
        std::cout << "1. List Drivers\n";
        std::cout << "2. Add Driver\n";
        std::cout << "3. Delete Driver\n";
        std::cout << "4. Filter Drivers\n";
        std::cout << "5. Remove Specific Driver Filters\n";
        std::cout << "6. Clear All Driver Filters\n";
        std::cout << "7. Edit Driver\n";
        std::cout << "8. Back\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: listDrivers();           break;
        case 2: addDriver();             break;
        case 3: deleteDriver();          break;
        case 4: filterDrivers();         break;
        case 5: removeDriverFilters();   break;
        case 6: clearAllDriverFilters(); break;
        case 7: editDriver();            break;
        case 8: return;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::listDrivers() {
    auto& drivers = dbManager.getDrivers();
    int count = 0;
    DriverTable::DriverInfo* filtered = drivers.applyFilters(count);

    // Определяем динамические ширины колонок на основе данных и заголовков:
    const std::string headerName = "Full Name";
    const std::string headerBirth = "Birth Date";
    const std::string headerCity = "City";

    int birthDateWidth = static_cast<int>(headerBirth.length()); // формат ДД.MM.YYYY всегда 10 символов, но пусть будет минимум под заголовок
    if (birthDateWidth < 10) birthDateWidth = 10;

    int fullNameWidth = static_cast<int>(headerName.length());
    int cityNameWidth = static_cast<int>(headerCity.length());

    // Сначала определяем максимальную длину "Full Name" и "City" среди отфильтрованных записей:
    if (filtered) {
        for (int i = 0; i < count; ++i) {
            const auto& di = filtered[i];
            if (static_cast<int>(di.fullName.length()) > fullNameWidth) {
                fullNameWidth = static_cast<int>(di.fullName.length());
            }
            // Получаем имя города по cityId
            std::string cityName = dbManager.getCities().getCityNameById(di.cityId);
            if (static_cast<int>(cityName.length()) > cityNameWidth) {
                cityNameWidth = static_cast<int>(cityName.length());
            }
        }
    }

    // Добавляем небольшой отступ (по 2 пробела) для читаемости
    fullNameWidth += 2;
    birthDateWidth += 2;
    cityNameWidth += 2;

    // Выводим верхнюю границу таблицы
    std::cout << "+"
        << std::string(fullNameWidth + 1, '-') << "+"
        << std::string(birthDateWidth + 1, '-') << "+"
        << std::string(cityNameWidth + 1, '-') << "+"
        << "\n";

    // Заголовок
    std::cout << "| " << std::left << std::setw(fullNameWidth) << headerName
        << "| " << std::left << std::setw(birthDateWidth) << headerBirth
        << "| " << std::left << std::setw(cityNameWidth) << headerCity
        << "|\n";

    // Разделитель после заголовка
    std::cout << "+"
        << std::string(fullNameWidth + 1, '-') << "+"
        << std::string(birthDateWidth + 1, '-') << "+"
        << std::string(cityNameWidth + 1, '-') << "+"
        << "\n";

    // Строки с данными
    if (filtered) {
        for (int i = 0; i < count; ++i) {
            const auto& di = filtered[i];
            std::string cityName = dbManager.getCities().getCityNameById(di.cityId);

            std::ostringstream oss;
            oss << "| " << std::left << std::setw(fullNameWidth) << di.fullName
                << "| " << std::left << std::setw(birthDateWidth) << di.birthDate
                << "| " << std::left << std::setw(cityNameWidth) << cityName
                << "|";
            std::cout << oss.str() << "\n";
        }
        delete[] filtered;
    }

    // Нижняя граница таблицы
    std::cout << "+"
        << std::string(fullNameWidth + 1, '-') << "+"
        << std::string(birthDateWidth + 1, '-') << "+"
        << std::string(cityNameWidth + 1, '-') << "+"
        << "\n";
}


void UserInterface::addDriver() {
    string fullName = readString("Full name: ");
    string birthDate = readString("Birth date (DD.MM.YYYY): ");
    string cityName = readString("City: ");

    int cityId = dbManager.getCities().getCityIdByName(cityName);
    if (cityId == -1) {
        // Город отсутствует — запросим параметры города
        std::cout << "City \"" << cityName << "\" not found. Please enter its data.\n";
        int population = readInt("Population: ");
        std::cout << "Population grade (0-Small, 1-Medium, 2-Large): ";
        int gradeInput = readInt("");
        auto grade = static_cast<CityTable::PopulationGrade>(gradeInput);
        std::cout << "Settlement type (0-City, 1-Town, 2-Village): ";
        int typeInput = readInt("");
        auto type = static_cast<CityTable::SettlementType>(typeInput);
        dbManager.addCity(cityName, population, grade, type);
        cityId = dbManager.getCities().getCityIdByName(cityName);
        std::cout << "City added automatically.\n";
    }

    try {
        dbManager.addDriver(fullName, birthDate, cityName);
        std::cout << "Driver added.\n";
    }
    catch (const exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void UserInterface::deleteDriver() {
    string name = readString("Full name to delete: ");
    auto candidates = dbManager.getDrivers().findAllByName(name);
    if (candidates.empty()) {
        std::cout << "No driver with that name.\n";
        return;
    }
    if (candidates.size() == 1) {
        dbManager.deleteDriverById(candidates[0].id);
        std::cout << "Driver deleted.\n";
        return;
    }
    // Если несколько с одинаковым ФИО — уточним по дате рождения
    std::cout << "Multiple drivers found with the same name.\n";
    string birthDate = readString("Birth date (DD.MM.YYYY): ");
    vector<DriverTable::DriverInfo> filtered;
    for (auto& d : candidates) {
        if (d.birthDate == birthDate) filtered.push_back(d);
    }
    if (filtered.empty()) {
        std::cout << "No driver with that birth date.\n";
        return;
    }
    if (filtered.size() == 1) {
        dbManager.deleteDriverById(filtered[0].id);
        std::cout << "Driver deleted.\n";
        return;
    }
    // Если по дате всё ещё несколько — уточним по городу
    std::cout << "Still multiple entries. Please specify city.\n";
    string cityName = readString("City: ");
    int cityId = dbManager.getCities().getCityIdByName(cityName);
    if (cityId == -1) {
        std::cout << "City not found.\n";
        return;
    }
    DriverTable::DriverInfo toDelete{ -1,"","",-1 };
    for (auto& d : filtered) {
        if (d.cityId == cityId) {
            toDelete = d;
            break;
        }
    }
    if (toDelete.id == -1) {
        std::cout << "No matching driver with that city.\n";
        return;
    }
    dbManager.deleteDriverById(toDelete.id);
    std::cout << "Driver deleted.\n";
}

void UserInterface::filterDrivers() {
    auto& drivers = dbManager.getDrivers();
    std::cout << "\nFilter by:\n";
    std::cout << "1. Full Name contains\n";
    std::cout << "2. Full Name equals\n";
    std::cout << "3. Birth Date equals\n";
    std::cout << "4. Cancel\n";
    int choice = readInt("Choose filter type: ");
    if (choice == 4) return;
    if (choice == 1) {
        string val = readString("Enter substring: ");
        drivers.addFilter("fullName", 1, val);
    }
    else if (choice == 2) {
        string val = readString("Enter full name: ");
        drivers.addFilter("fullName", 2, val);
    }
    else if (choice == 3) {
        string val = readString("Enter birth date (DD.MM.YYYY): ");
        drivers.addFilter("birthDate", 2, val);
    }
    else {
        std::cout << "Invalid option.\n";
        return;
    }
    std::cout << "Filter added.\n";
}

void UserInterface::removeDriverFilters() {
    auto& drivers = dbManager.getDrivers();
    while (true) {
        int count = drivers.getFilterCount();
        if (count == 0) {
            std::cout << "No active filters to remove.\n";
            return;
        }
        std::cout << "\nActive Filters:\n";
        for (int i = 0; i < count; ++i) {
            std::cout << i + 1 << ". " << drivers.getFilterDescription(i) << "\n";
        }
        std::cout << "0. Remove ALL filters\n";
        std::cout << "-1. Done removing\n";
        int choice = readInt("Choose option: ");
        if (choice == -1) {
            break;
        }
        else if (choice == 0) {
            drivers.clearFilters();
            std::cout << "All filters removed.\n";
            break;
        }
        else if (choice >= 1 && choice <= count) {
            drivers.removeFilterAt(choice - 1);
            std::cout << "Filter removed.\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::clearAllDriverFilters() {
    dbManager.getDrivers().clearFilters();
    std::cout << "All driver filters cleared.\n";
}

void UserInterface::editDriver() {
    listDrivers();
    string name = readString("Enter Driver full name to edit: ");
    auto candidates = dbManager.getDrivers().findAllByName(name);
    if (candidates.empty()) {
        std::cout << "Driver not found.\n";
        return;
    }
    DriverTable::DriverInfo target{};
    if (candidates.size() == 1) {
        target = candidates[0];
    }
    else {
        // Уточняем по дате рождения
        std::cout << "Multiple drivers found. Enter Birth Date (DD.MM.YYYY): ";
        string birthDate = readString("");
        vector<DriverTable::DriverInfo> filtered;
        for (auto& d : candidates) {
            if (d.birthDate == birthDate) filtered.push_back(d);
        }
        if (filtered.size() == 1) {
            target = filtered[0];
        }
        else if (filtered.empty()) {
            std::cout << "No driver with that birth date.\n";
            return;
        }
        else {
            // Уточняем по городу
            std::cout << "Still multiple entries. Enter City: ";
            string cityName = readString("");
            int cityId = dbManager.getCities().getCityIdByName(cityName);
            if (cityId == -1) {
                std::cout << "City not found.\n";
                return;
            }
            for (auto& d : filtered) {
                if (d.cityId == cityId) {
                    target = d;
                    break;
                }
            }
            if (target.id == -1) {
                std::cout << "No matching driver with that city.\n";
                return;
            }
        }
    }

    std::cout << "\nEdit field:\n";
    std::cout << "1. Full Name\n";
    std::cout << "2. Birth Date\n";
    std::cout << "3. City\n";
    std::cout << "4. Cancel\n";
    int choice = readInt("Choose field: ");
    auto& drivers = dbManager.getDrivers();
    if (choice == 1) {
        string newName = readString("Enter new name: ");
        if (drivers.updateDriverName(target.id, newName))
            std::cout << "Name updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 2) {
        string newDate = readString("Enter new birth date (DD.MM.YYYY): ");
        if (drivers.updateDriverBirthDate(target.id, newDate))
            std::cout << "Birth date updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 3) {
        string newCity = readString("Enter new city: ");
        int newCityId = dbManager.getCities().getCityIdByName(newCity);
        if (newCityId == -1) {
            std::cout << "City not found.\n";
        }
        else {
            if (drivers.updateDriverCity(target.id, newCityId)) {
                // Каскадно обновляем все нарушения этого водителя
                dbManager.getRegistry().updateViolationsCity(target.id, newCityId);
                std::cout << "City updated (and all related violations updated).\n";
            }
            else {
                std::cout << "Update failed.\n";
            }
        }
    }
    else {
        std::cout << "Cancel.\n";
    }
    dbManager.saveAll();
}

// ==================== Fines ====================
void UserInterface::finesMenu() {
    while(true){
        std::cout << "\n--- Fines ---\n";
        std::cout << "1. List Fines\n";
        std::cout << "2. Add Fine\n";
        std::cout << "3. Delete Fine\n";
        std::cout << "4. Filter Fines\n";
        std::cout << "5. Remove Specific Fine Filters\n";
        std::cout << "6. Clear All Fine Filters\n";
        std::cout << "7. Edit Fine\n";
        std::cout << "8. Back\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: listFines();           break;
        case 2: addFine();             break;
        case 3: deleteFine();          break;
        case 4: filterFines();         break;
        case 5: removeFineFilters();   break;
        case 6: clearAllFineFilters(); break;
        case 7: editFine();            break;
        case 8: return;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::listFines() {
    auto& fines = dbManager.getFines();
    int count = 0;
    FineTable::FineInfo* filtered = fines.applyFilters(count);
    std::cout << "+------------+------------------------------+----------+\n";
    std::cout << "| Amount     | Type                         | Severity |\n";
    std::cout << "+------------+------------------------------+----------+\n";
    if (filtered) {
        for (int i = 0; i < count; ++i) {
            auto& fi = filtered[i];
            ostringstream oss;
            oss << "| " << right << setw(10) << fi.amount << " | "
                << left << setw(28) << fi.type << " | "
                << left << setw(8) << FineTable::severityToString(fi.severity) << " |";
            std::cout << oss.str() << "\n";
        }
        delete[] filtered;
    }
    std::cout << "+------------+------------------------------+----------+\n";
}

void UserInterface::addFine() {
    string type = readString("Fine type: ");
    double amount = readDouble("Amount: ");
    std::cout << "Severity (0-Light, 1-Medium, 2-Heavy): ";
    int s = readInt("");
    try {
        dbManager.addFine(type, amount, static_cast<FineTable::Severity>(s));
        std::cout << "Fine added.\n";
    }
    catch (const exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void UserInterface::deleteFine() {
    string type = readString("Fine type to delete: ");
    dbManager.getFines().deleteFine(type);
    std::cout << "Fine deleted (if existed).\n";
}

void UserInterface::filterFines() {
    auto& fines = dbManager.getFines();
    std::cout << "\nFilter by:\n";
    std::cout << "1. Type contains\n";
    std::cout << "2. Type equals\n";
    std::cout << "3. Amount <\n";
    std::cout << "4. Amount >\n";
    std::cout << "5. Amount equals\n";
    std::cout << "6. Severity equals\n";
    std::cout << "7. Cancel\n";
    int choice = readInt("Choose filter type: ");
    if (choice == 7) return;
    if (choice == 1) {
        string val = readString("Enter substring: ");
        fines.addFilter("type", 1, val);
    }
    else if (choice == 2) {
        string val = readString("Enter full type: ");
        fines.addFilter("type", 2, val);
    }
    else if (choice == 3) {
        string val = readString("Enter threshold: ");
        fines.addFilter("amount", 3, val);
    }
    else if (choice == 4) {
        string val = readString("Enter threshold: ");
        fines.addFilter("amount", 4, val);
    }
    else if (choice == 5) {
        string val = readString("Enter exact value: ");
        fines.addFilter("amount", 2, val);
    }
    else if (choice == 6) {
        string val = readString("Enter severity: ");
        fines.addFilter("severity", 2, val);
    }
    else {
        std::cout << "Invalid option.\n";
        return;
    }
    std::cout << "Filter added.\n";
}

void UserInterface::removeFineFilters() {
    auto& fines = dbManager.getFines();
    while (true) {
        int count = fines.getFilterCount();
        if (count == 0) {
            std::cout << "No active filters to remove.\n";
            return;
        }
        std::cout << "\nActive Filters:\n";
        for (int i = 0; i < count; ++i) {
            std::cout << i + 1 << ". " << fines.getFilterDescription(i) << "\n";
        }
        std::cout << "0. Remove ALL filters\n";
        std::cout << "-1. Done removing\n";
        int choice = readInt("Choose option: ");
        if (choice == -1) {
            break;
        }
        else if (choice == 0) {
            fines.clearFilters();
            std::cout << "All filters removed.\n";
            break;
        }
        else if (choice >= 1 && choice <= count) {
            fines.removeFilterAt(choice - 1);
            std::cout << "Filter removed.\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::clearAllFineFilters() {
    dbManager.getFines().clearFilters();
    std::cout << "All fine filters cleared.\n";
}

void UserInterface::editFine() {
    listFines();
    string type = readString("Enter Fine type to edit: ");
    auto& fines = dbManager.getFines();
    int id = fines.getFineIdByType(type);
    if (id == -1) {
        std::cout << "Fine not found.\n";
        return;
    }
    std::cout << "\nEdit field:\n";
    std::cout << "1. Type\n";
    std::cout << "2. Amount\n";
    std::cout << "3. Severity\n";
    std::cout << "4. Cancel\n";
    int choice = readInt("Choose field: ");
    if (choice == 1) {
        string newType = readString("Enter new type: ");
        if (fines.updateFineType(id, newType))
            std::cout << "Type updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 2) {
        double newAmt = readDouble("Enter new amount: ");
        if (fines.updateFineAmount(id, newAmt))
            std::cout << "Amount updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else if (choice == 3) {
        std::cout << "Severity (0-Light, 1-Medium, 2-Heavy): ";
        int snew = readInt("");
        if (fines.updateFineSeverity(id, static_cast<FineTable::Severity>(snew)))
            std::cout << "Severity updated.\n";
        else
            std::cout << "Update failed.\n";
    }
    else {
        std::cout << "Cancel.\n";
    }
    dbManager.saveAll();
}

// ==================== Violations ====================
void UserInterface::registryMenu() {
    while(true){
        std::cout << "\n--- Violations ---\n";
        std::cout << "1. List Violations\n";
        std::cout << "2. Add Violation\n";
        std::cout << "3. Mark Paid\n";
        std::cout << "4. Filter Violations\n";
        std::cout << "5. Remove Specific Violation Filters\n";
        std::cout << "6. Clear All Violation Filters\n";
        std::cout << "7. Edit Violation\n";
        std::cout << "8. Back\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: listViolations();            break;
        case 2: addViolation();              break;
        case 3: markViolationPaid();         break;
        case 4: filterViolations();          break;
        case 5: removeViolationFilters();    break;
        case 6: clearAllViolationFilters();  break;
        case 7: editViolation();             break;
        case 8: return;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::listViolations() {
    auto violations = dbManager.getRegistry().applyFilters(
        dbManager.getDrivers(),
        dbManager.getCities(),
        dbManager.getFines()
    );
    std::cout << "+---------------------------------------+-----------------+-----------------+-------------+------+----------+\n";
    std::cout << "| Driver                                | City            | Fine            | Date        | Paid | Amount   |\n";
    std::cout << "+---------------------------------------+-----------------+-----------------+-------------+------+----------+\n";
    for (auto& v : violations) {
        ostringstream oss;
        oss << "| " << left << setw(37) << v.driverName << " | "
            << left << setw(15) << v.cityName << " | "
            << left << setw(15) << v.fineType << " | "
            << left << setw(11) << v.date << " | "
            << left << setw(4) << (v.paid ? "Yes" : "No") << " | "
            << right << setw(8) << v.fineAmount << " |";
        std::cout << oss.str() << "\n";
    }
    std::cout << "+---------------------------------------+-----------------+-----------------+-------------+------+----------+\n";
}

void UserInterface::addViolation() {
    // Сначала выбираем водителя:
    listDrivers();
    string driverName = readString("Driver full name: ");

    // Найдём всех кандидатов по ФИО:
    auto candidates = dbManager.getDrivers().findAllByName(driverName);
    if (candidates.empty()) {
        std::cout << "No driver with that name.\n";
        return;
    }
    DriverTable::DriverInfo chosen{};
    if (candidates.size() == 1) {
        chosen = candidates[0];
    }
    else {
        // Уточняем по дате рождения
        std::cout << "Multiple drivers found. Enter Birth Date (DD.MM.YYYY): ";
        string bdate = readString("");
        vector<DriverTable::DriverInfo> filtered;
        for (auto& d : candidates) {
            if (d.birthDate == bdate) filtered.push_back(d);
        }
        if (filtered.size() == 1) {
            chosen = filtered[0];
        }
        else if (filtered.empty()) {
            std::cout << "No driver with that birth date.\n";
            return;
        }
        else {
            // Уточняем по городу
            std::cout << "Still multiple entries. Enter City: ";
            string cityName = readString("");
            int cityId = dbManager.getCities().getCityIdByName(cityName);
            if (cityId == -1) {
                std::cout << "City not found.\n";
                return;
            }
            for (auto& d : filtered) {
                if (d.cityId == cityId) {
                    chosen = d;
                    break;
                }
            }
            if (chosen.id == 0) {
                std::cout << "No matching driver with that city.\n";
                return;
            }
        }
    }

    // Выбираем штраф
    listFines();
    string fineType = readString("Fine type: ");
    int fineId = dbManager.getFines().getFineIdByType(fineType);
    if (fineId == -1) {
        std::cout << "Fine not found.\n";
        return;
    }

    // Вводим дату нарушения
    string dateStr = readString("Date (DD.MM.YYYY): ");
    Date violationDate;
    if (!parseDate(dateStr, violationDate) || !isDateValid(violationDate)) {
        std::cout << "Invalid date format.\n";
        return;
    }
    // Проверяем возраст водителя на момент нарушения
    Date birthDate;
    if (!parseDate(chosen.birthDate, birthDate) || !isDateValid(birthDate)) {
        std::cout << "Stored driver birth date invalid.\n";
        return;
    }
    int ageAtViolation = calculateAge(birthDate, violationDate);
    if (ageAtViolation < 18) {
        std::cout << "Driver was under 18 at that date. Cannot add violation.\n";
        return;
    }

    // Всё ок, добавляем
    try {
        dbManager.getRegistry().addViolation(chosen.id, chosen.cityId, fineId, dateStr);
        std::cout << "Violation added.\n";
    }
    catch (const exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    dbManager.saveAll();
}

void UserInterface::markViolationPaid() {
    string date = readString("Enter violation date to mark paid: ");
    string driverName = readString("Enter driver full name: ");
    string fineType = readString("Enter fine type: ");
    auto violations = dbManager.getAllViolations();
    bool found = false;
    for (auto& v : violations) {
        if (v.driverName == driverName && v.fineType == fineType && v.date == date && !v.paid) {
            dbManager.markFineAsPaid(v.recordId);
            found = true;
        }
    }
    if (found) {
        std::cout << "Violation(s) marked paid.\n";
        dbManager.saveAll();
    }
    else {
        std::cout << "No unpaid violation found matching criteria.\n";
    }
}

void UserInterface::filterViolations() {
    auto& registry = dbManager.getRegistry();
        std::cout << "\n--- Violation Filters ---\n";
        std::cout << "1. By driver name\n";
        std::cout << "2. By city\n";
        std::cout << "3. By fine type\n";
        std::cout << "4. By payment status\n";
        std::cout << "5. By amount\n";
        std::cout << "7. Remove specific filter\n";
        std::cout << "8. Clear all filters\n";
        std::cout << "9. Back\n";

        int choice = readInt("Choose option: ");
        if (choice == 8) return;

        std::string val;
        int cmpType;

        switch (choice) {
        case 1: {
            val = readString("Enter driver's full name: ");
            registry.addFilter("driver", 2, val);
            break;
        }
        case 2: {
            val = readString("Enter city name: ");
            registry.addFilter("city", 2, val);
            break;
        }
        case 3: {
            val = readString("Enter fine type: ");
            registry.addFilter("fineType", 2, val);
            break;
        }
        case 4: {
            int paid = readInt("Paid? (1 - Yes, 0 - No): ");
            registry.addFilter("paid", 2, paid ? "1" : "0");
            break;
        }
        case 5: {
            cmpType = readInt("Compare as (3 - Less, 4 - Greater): ");
            val = readString("Enter amount: ");
            registry.addFilter("amount", cmpType, val);
            break;
        }
        case 6:
            removeViolationFilters();
            break;
        case 7:
            registry.clearFilters();
            std::cout << "All filters cleared.\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
        }

        if (choice >= 1 && choice <= 6) {
            std::cout << "Filter added: "
                << registry.getFilterDescription(registry.getFilterCount() - 1)
                << "\n";
        }
}

void UserInterface::removeViolationFilters() {
    auto& registry = dbManager.getRegistry();
    int count = registry.getFilterCount();

    if (count == 0) {
        std::cout << "No active filters.\n";
        return;
    }

    std::cout << "\nActive Filters:\n";
    for (int i = 0; i < count; i++) {
        std::cout << i + 1 << ". " << registry.getFilterDescription(i) << "\n";
    }

    int choice = readInt("Enter filter number to remove (0 to cancel): ");
    if (choice > 0 && choice <= count) {
        registry.removeFilterAt(choice - 1);
        std::cout << "Filter removed.\n";
    }
}

void UserInterface::clearAllViolationFilters() {
    dbManager.getRegistry().clearFilters();
    std::cout << "All violation filters cleared.\n";
}

// ======== Edit Violation ========
void UserInterface::editViolation() {
    listViolations();
    std::cout << "\nTo edit a violation, specify:\n";
    string driverName = readString("  Driver full name: ");
    string fineType = readString("  Fine type: ");
    string dateStr = readString("  Date (DD.MM.YYYY): ");

    // Ищем все нарушения, удовлетворяющие этим полям
    auto allV = dbManager.getAllViolations();
    vector<FineRegistry::ViolationInfo> candidates;
    for (auto& v : allV) {
        if (v.driverName == driverName && v.fineType == fineType && v.date == dateStr) {
            candidates.push_back(v);
        }
    }
    if (candidates.empty()) {
        std::cout << "No matching violation found.\n";
        return;
    }
    FineRegistry::ViolationInfo chosen = candidates[0];
    if (candidates.size() > 1) {
        std::cout << "Multiple violations matched. Showing recordIds:\n";
        for (auto& v : candidates) {
            std::cout << "  Record ID: " << v.recordId << "\n";
        }
        int rid = readInt("Enter Record ID to edit: ");
        bool foundId = false;
        for (auto& v : candidates) {
            if (v.recordId == rid) {
                chosen = v;
                foundId = true;
                break;
            }
        }
        if (!foundId) {
            std::cout << "Record ID not found among candidates.\n";
            return;
        }
    }

    std::cout << "\nEditing Violation Record ID = " << chosen.recordId << "\n";
    std::cout << "1. Change Driver\n";
    std::cout << "2. Change Fine Type\n";
    std::cout << "3. Change Date\n";
    std::cout << "4. Change Paid Status\n";
    std::cout << "5. Cancel\n";
    int choice = readInt("Choose field: ");

    auto& registry = dbManager.getRegistry();
    auto& drivers = dbManager.getDrivers();
    auto& cities = dbManager.getCities();
    auto& fines = dbManager.getFines();

    if (choice == 1) {
        // Сменить водителя
        listDrivers();
        string newDriverName = readString("Enter new driver full name: ");
        auto newCandidates = drivers.findAllByName(newDriverName);
        if (newCandidates.empty()) {
            std::cout << "No driver with that name.\n";
            return;
        }
        DriverTable::DriverInfo newChosen{};
        if (newCandidates.size() == 1) {
            newChosen = newCandidates[0];
        }
        else {
            // Уточняем по дате рождения
            std::cout << "Multiple drivers found. Enter Birth Date (DD.MM.YYYY): ";
            string bd = readString("");
            vector<DriverTable::DriverInfo> f2;
            for (auto& d : newCandidates) {
                if (d.birthDate == bd) f2.push_back(d);
            }
            if (f2.size() == 1) {
                newChosen = f2[0];
            }
            else if (f2.empty()) {
                std::cout << "No driver with that birth date.\n";
                return;
            }
            else {
                // Уточняем по городу
                std::cout << "Still multiple entries. Enter City: ";
                string cityName = readString("");
                int cityId = cities.getCityIdByName(cityName);
                if (cityId == -1) {
                    std::cout << "City not found.\n";
                    return;
                }
                for (auto& d : f2) {
                    if (d.cityId == cityId) {
                        newChosen = d;
                        break;
                    }
                }
                if (newChosen.id == 0) {
                    std::cout << "No matching driver with that city.\n";
                    return;
                }
            }
        }
        // Проверяем возраст на старую дату нарушения
        Date oldViolationDate;
        if (!parseDate(chosen.date, oldViolationDate) || !isDateValid(oldViolationDate)) {
            std::cout << "Stored violation date invalid.\n";
            return;
        }
        Date newBirthDate;
        if (!parseDate(newChosen.birthDate, newBirthDate) || !isDateValid(newBirthDate)) {
            std::cout << "Stored new driver birth date invalid.\n";
            return;
        }
        int newAge = calculateAge(newBirthDate, oldViolationDate);
        if (newAge < 18) {
            std::cout << "New driver was under 18 at that date. Cannot reassign violation.\n";
            return;
        }
        // Всё ок, обновляем driverId и привязываем новую cityId
        registry.updateViolationDriver(chosen.recordId, newChosen.id, newChosen.cityId);
        std::cout << "Driver in violation updated.\n";
    }
    else if (choice == 2) {
        // Сменить fineType
        listFines();
        string newFineType = readString("Enter new fine type: ");
        int newFineId = fines.getFineIdByType(newFineType);
        if (newFineId == -1) {
            std::cout << "Fine not found.\n";
            return;
        }
        registry.updateViolationFine(chosen.recordId, newFineId);
        std::cout << "Fine type updated.\n";
    }
    else if (choice == 3) {
        // Сменить дату
        string newDate = readString("Enter new date (DD.MM.YYYY): ");
        Date nd;
        if (!parseDate(newDate, nd) || !isDateValid(nd)) {
            std::cout << "Invalid date format.\n";
            return;
        }
        // Проверяем возраст водителя на новую дату
        Date birthDate;
        auto drvInfo = drivers.getDriverId(chosen.driverName);
        // Найдём дату рождения текущего водителя
        DriverTable::DriverInfo di{};
        drivers.driverIteratorReset();
        while (drivers.driverIteratorHasNext()) {
            auto info = drivers.driverIteratorNext();
            if (info.id == chosen.driverId) {
                di = info;
                break;
            }
        }
        if (!parseDate(di.birthDate, birthDate) || !isDateValid(birthDate)) {
            std::cout << "Stored driver birth date invalid.\n";
            return;
        }
        int newAge = calculateAge(birthDate, nd);
        if (newAge < 18) {
            std::cout << "Driver was under 18 at that new date. Cannot set violation date.\n";
            return;
        }
        registry.updateViolationDate(chosen.recordId, newDate);
        std::cout << "Violation date updated.\n";
    }
    else if (choice == 4) {
        // Сменить статус оплаты
        std::cout << "Current paid status: " << (chosen.paid ? "Yes" : "No") << "\n";
        std::cout << "Enter new status (1 = paid, 0 = unpaid): ";
        int p = readInt("");
        registry.updateViolationPaid(chosen.recordId, p == 1);
        std::cout << "Paid status updated.\n";
    }
    else {
        std::cout << "Cancelled.\n";
        return;
    }
    dbManager.saveAll();
}

// ==================== Statistics ====================
void UserInterface::statisticsMenu() {
    while(true){
        std::cout << "\n--- Statistics ---\n";
        std::cout << "1. Violations by City\n";
        std::cout << "2. Top-5 Drivers\n";
        std::cout << "3. Back\n";
        int choice = readInt("Choose option: ");
        switch (choice) {
        case 1: showViolationsByCity(); break;
        case 2: showTopDrivers();       break;
        case 3: return;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

void UserInterface::showViolationsByCity() {
    CityViolations stats[MAX_CITIES];
    int cityCount = 0;
    collectCityStats(stats, cityCount);
    if (cityCount == 0) {
        std::cout << "No cities with violations.\n";
        return;
    }
    qsort(stats, cityCount, sizeof(CityViolations), compareCityStats);
    printCityViolations(stats, cityCount);
}

void UserInterface::collectCityStats(CityViolations* stats, int& cityCount) {
    cityCount = 0;
    auto violations = dbManager.getAllViolations();
    for (auto& v : violations) {
        int idx = -1;
        for (int i = 0; i < cityCount; ++i) {
            if (stats[i].name == v.cityName) {
                idx = i;
                break;
            }
        }
        if (idx == -1) {
            idx = cityCount++;
            stats[idx].name = v.cityName;
            stats[idx].count = 0;
        }
        stats[idx].violationIds[stats[idx].count++] = v.recordId;
    }
}

// ======= Утилиты ввода/вывода =======
int UserInterface::readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Invalid input. Enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
    }
}

double UserInterface::readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Invalid input. Enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string UserInterface::readString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(cin, value);
    return value;
}
