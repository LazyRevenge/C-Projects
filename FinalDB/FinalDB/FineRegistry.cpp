#include "FineRegistry.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
using namespace std;

// Конструктор: инициализация заголовочного узла и загрузка данных
FineRegistry::FineRegistry()
    : head(new ViolationNode(-1, -1, -1, -1, false, "", nullptr)),
    currentIterator(nullptr),
    recordIdWidth(5),
    driverIdWidth(5),
    cityIdWidth(5),
    fineIdWidth(5),
    paidWidth(8),
    dateWidth(12)
{
    loadFromFile();
}

// Деструктор: очистка списка и хеш-таблицы
FineRegistry::~FineRegistry() {
    ViolationNode* current = head->next;
    while (current) {
        ViolationNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete head;
    recordToNodeMap.clear();
}

// Загрузка данных из файла
bool FineRegistry::loadFromFile() {
    return loadFromFile("registry.txt");
}

bool FineRegistry::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening registry file: " << filename << "\n";
        return false;
    }
    // Очистка
    ViolationNode* curr = head->next;
    while (curr) {
        ViolationNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head->next = nullptr;
    recordToNodeMap.clear();

    std::string line;
    while (std::getline(file, line)) parseLine(line);
    file.close();
    return true;
}

// Парсинг строки: "recordId driverId cityId fineId paid date"
void FineRegistry::parseLine(const std::string& line) {
    if (line.empty()) return;
    istringstream iss(line);
    int recordId, driverId, cityId, fineId;
    int paidInt;
    string date;
    iss >> recordId >> driverId >> cityId >> fineId >> paidInt >> quoted(date);

    bool paid = (paidInt == 1);
    addViolationNode(recordId, driverId, cityId, fineId, paid, date);
}

// Добавление узла в список и в хеш-таблицу
void FineRegistry::addViolationNode(int recordId, int driverId, int cityId,
    int fineId, bool paid, const std::string& date)
{
    ViolationNode* newNode = new ViolationNode(
        recordId, driverId, cityId, fineId, paid, date, head->next
    );
    head->next = newNode;
    recordToNodeMap.insert(recordId, newNode);
}

// Добавление нового нарушения (с генерацией recordId)
void FineRegistry::addViolation(int driverId, int cityId, int fineId, const std::string& date) {
    // Генерация recordId: максимум существующего +1
    int newId = 1;
    ViolationNode* curr = head->next;
    while (curr) {
        if (curr->recordId >= newId) newId = curr->recordId + 1;
        curr = curr->next;
    }
    addViolationNode(newId, driverId, cityId, fineId, false, date);
}

// Итератор: сброс на начало
void FineRegistry::violationIteratorReset() const {
    currentIterator = head->next;
}

// Есть ли следующий?
bool FineRegistry::violationIteratorHasNext() const {
    return currentIterator != nullptr;
}

// Получение следующего с детализированной информацией
FineRegistry::ViolationInfo FineRegistry::violationIteratorNext(
    const DriverTable& drivers, const CityTable& cities, const FineTable& fines) const
{
    ViolationInfo info;
    if (!currentIterator) return info;
    info.recordId = currentIterator->recordId;
    info.driverId = currentIterator->driverId;
    info.cityId = currentIterator->cityId;
    info.fineId = currentIterator->fineId;
    info.paid = currentIterator->paid;
    info.date = currentIterator->date;

    // Получаем driverName
    info.driverName = drivers.getDriverNameById(info.driverId);

    // Получаем cityName
    info.cityName = cities.getCityNameById(info.cityId);

    // Получаем fineType и fineAmount
    info.fineType = fines.getFineTypeById(info.fineId);
    info.fineAmount = fines.getAmountById(info.fineId);

    currentIterator = currentIterator->next;
    return info;
}

// Получить одно нарушение по recordId
FineRegistry::ViolationInfo FineRegistry::getViolationById(
    int recordId,
    const DriverTable& drivers,
    const CityTable& cities,
    const FineTable& fines) const
{
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (!node) return ViolationInfo{};
    ViolationInfo info;
    info.recordId = node->recordId;
    info.driverId = node->driverId;
    info.cityId = node->cityId;
    info.fineId = node->fineId;
    info.paid = node->paid;
    info.date = node->date;

    info.driverName = drivers.getDriverNameById(info.driverId);
    info.cityName = cities.getCityNameById(info.cityId);
    info.fineType = fines.getFineTypeById(info.fineId);
    info.fineAmount = fines.getAmountById(info.fineId);

    return info;
}

// Пометка оплаченным
void FineRegistry::markAsPaid(int recordId) {
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (node) {
        node->paid = true;
    }
}

// Обновление ссылок при удалении водителя (driverId = -1)
void FineRegistry::updateDriverReferences(int deletedDriverId) {
    ViolationNode* curr = head->next;
    while (curr) {
        if (curr->driverId == deletedDriverId) {
            curr->driverId = -1;
        }
        curr = curr->next;
    }
}

// Обновление ссылок при удалении города (cityId = -1)
void FineRegistry::updateCityReferences(int deletedCityId) {
    ViolationNode* curr = head->next;
    while (curr) {
        if (curr->cityId == deletedCityId) {
            curr->cityId = -1;
        }
        curr = curr->next;
    }
}

// Обновление cityId у всех нарушений данного водителя
void FineRegistry::updateViolationsCity(int driverId, int newCityId) {
    ViolationNode* curr = head->next;
    while (curr) {
        if (curr->driverId == driverId) {
            curr->cityId = newCityId;
        }
        curr = curr->next;
    }
}

// ======== НОВЫЕ МЕТОДЫ ДЛЯ РЕДАКТИРОВАНИЯ НАРУШЕНИЯ ========

// Изменить водителя (и cityId) у записи нарушения
bool FineRegistry::updateViolationDriver(int recordId, int newDriverId, int newCityId) {
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (!node) return false;
    node->driverId = newDriverId;
    node->cityId = newCityId;
    return true;
}

// Изменить тип штрафа (fineId)
bool FineRegistry::updateViolationFine(int recordId, int newFineId) {
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (!node) return false;
    node->fineId = newFineId;
    return true;
}

// Изменить дату нарушения
bool FineRegistry::updateViolationDate(int recordId, const std::string& newDate) {
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (!node) return false;
    node->date = newDate;
    return true;
}

// Изменить статус оплаты
bool FineRegistry::updateViolationPaid(int recordId, bool paid) {
    ViolationNode* node = recordToNodeMap.find<ViolationNode>(recordId);
    if (!node) return false;
    node->paid = paid;
    return true;
}

// Сохранение данных в файл
void FineRegistry::saveToFile() const {
    ofstream file("registry.txt");
    if (!file.is_open()) {
        cerr << "Error opening registry file for writing!" << endl;
        return;
    }
    ViolationNode* curr = head->next;
    while (curr) {
        file << curr->recordId << ' '
            << curr->driverId << ' '
            << curr->cityId << ' '
            << curr->fineId << ' '
            << (curr->paid ? 1 : 0) << ' '
            << quoted(curr->date) << '\n';
        curr = curr->next;
    }
    file.close();
}
//========== РАБОТА С ФИЛЬТРОМ ==========
void FineRegistry::addFilter(const std::string& field, int cmpType, const std::string& value) {
    Filter* newFilter = new Filter{ field, cmpType, value, nullptr };
    if (!violationFilters) {
        violationFilters = newFilter;
    }
    else {
        Filter* temp = violationFilters;
        while (temp->next) temp = temp->next;
        temp->next = newFilter;
    }
}

void FineRegistry::clearFilters() {
    Filter* current = violationFilters;
    while (current) {
        Filter* next = current->next;
        delete current;
        current = next;
    }
    violationFilters = nullptr;
}

int FineRegistry::getFilterCount() const {
    int count = 0;
    Filter* temp = violationFilters;
    while (temp) {
        count++;
        temp = temp->next;
    }
    return count;
}

std::string FineRegistry::getFilterDescription(int index) const {
    int i = 0;
    Filter* temp = violationFilters;
    while (temp) {
        if (i == index) {
            std::ostringstream oss;
            if (temp->field == "driver") {
                oss << "Driver: " << temp->value;
            }
            else if (temp->field == "city") {
                oss << "City: " << temp->value;
            }
            else if (temp->field == "fineType") {
                oss << "Fine type: " << temp->value;
            }
            else if (temp->field == "paid") {
                oss << "Paid: " << temp->value;
            }
            else if (temp->field == "amount") {
                oss << "Amount " << (temp->cmpType == 3 ? "< " : "> ") << temp->value;
            }
            return oss.str();
        }
        temp = temp->next;
        i++;
    }
    return "";
}

void FineRegistry::removeFilterAt(int index) {
    if (index == 0) {
        Filter* toDelete = violationFilters;
        violationFilters = violationFilters->next;
        delete toDelete;
        return;
    }

    int i = 0;
    Filter* prev = violationFilters;
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

std::vector<FineRegistry::ViolationInfo> FineRegistry::applyFilters(
    const DriverTable& drivers,
    const CityTable& cities,
    const FineTable& fines) const
{
    std::vector<ViolationInfo> result;
    ViolationNode* current = head->next;

    while (current) {
        if (matchFilter(current, drivers, cities, fines)) {
            result.push_back(getViolationInfo(current, drivers, cities, fines));
        }
        current = current->next;
    }

    return result;
}

static int dateToInt(const std::string& dateStr) {
    try {
        return stoi(dateStr.substr(6, 4)) * 10000
            + stoi(dateStr.substr(3, 2)) * 100
            + stoi(dateStr.substr(0, 2));
    }
    catch (...) {
        return 0;
    }
}

bool FineRegistry::matchFilter(const ViolationNode* node,
    const DriverTable& drivers,
    const CityTable& cities,
    const FineTable& fines) const
{
    Filter* currentFilter = violationFilters;
    while (currentFilter) {
        bool match = false;
        ViolationInfo vi = getViolationInfo(node, drivers, cities, fines);

        if (currentFilter->field == "driver") {
            match = (vi.driverName == currentFilter->value);
        }
        else if (currentFilter->field == "city") {
            match = (vi.cityName == currentFilter->value);
        }
        else if (currentFilter->field == "fineType") {
            match = (vi.fineType == currentFilter->value);
        }
        else if (currentFilter->field == "paid") {
            match = (vi.paid == (currentFilter->value == "1"));
        }
        else if (currentFilter->field == "amount") {
            double amount = vi.fineAmount;
            double filterAmount = stod(currentFilter->value);
            match = (currentFilter->cmpType == 3) ? (amount < filterAmount)
                : (amount > filterAmount);
        }
        else if (currentFilter->field == "date") {
            int nodeDate = dateToInt(vi.date);
            int filterDate = dateToInt(currentFilter->value);

            if (currentFilter->cmpType == 3) match = (nodeDate < filterDate);
            else if (currentFilter->cmpType == 4) match = (nodeDate > filterDate);
        }

        if (!match) return false;
        currentFilter = currentFilter->next;
    }
    return true;
}

FineRegistry::ViolationInfo FineRegistry::getViolationInfo(
    const ViolationNode* node,
    const DriverTable& drivers,
    const CityTable& cities,
    const FineTable& fines) const
{
    ViolationInfo info;
    info.recordId = node->recordId;
    info.driverId = node->driverId;
    info.cityId = node->cityId;
    info.fineId = node->fineId;
    info.paid = node->paid;
    info.date = node->date;

    // Получаем детали из связанных таблиц
    info.driverName = drivers.getDriverNameById(node->driverId);
    info.cityName = cities.getCityNameById(node->cityId);
    info.fineType = fines.getFineTypeById(node->fineId);
    info.fineAmount = fines.getAmountById(node->fineId);

    return info;
}