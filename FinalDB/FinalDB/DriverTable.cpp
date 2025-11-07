#include "DriverTable.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
using namespace std;

// Вспомогательные: разбирают дату
static bool parseDateParts(const string& dateStr, int& day, int& month, int& year) {
    if (dateStr.size() != 10) return false;
    if (dateStr[2] != '.' || dateStr[5] != '.') return false;
    try {
        day = stoi(dateStr.substr(0, 2)); //(start,length)
        month = stoi(dateStr.substr(3, 2));
        year = stoi(dateStr.substr(6, 4));
    }
    catch (...) {
        return false;
    }
    return true;
}

static int calculateAgeFromBirth(const string& birthDateStr) {
    int day, month, year;
    if (!parseDateParts(birthDateStr, day, month, year)) return -1;
    time_t t = time(nullptr);
    tm nowStruct;
    localtime_s(&nowStruct, &t);
    int cy = nowStruct.tm_year + 1900;
    int cm = nowStruct.tm_mon + 1;
    int cd = nowStruct.tm_mday;
    int age = cy - year;
    if (cm < month || (cm == month && cd < day)) age--;
    return age;
}

DriverTable::DriverTable()
    : head(new DriverNode(-1, "", "", -1, nullptr)),
    currentIterator(nullptr),
    currentFilter(nullptr),
    idWidth(5),
    nameWidth(30),
    birthDateWidth(12),
    cityIdWidth(5)
{
    loadFromFile();
}

DriverTable::~DriverTable() {
    DriverNode* current = head->next;
    while (current) {
        DriverNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete head;
    idToDriverMap.clear();
    nameToIdMap.clear();

    Filter* f = currentFilter;
    while (f) {
        Filter* nxt = f->next;
        delete f;
        f = nxt;
    }
}

// Загрузка из файла
bool DriverTable::loadFromFile() {
    return loadFromFile("drivers.txt");
}

bool DriverTable::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening drivers file: " << filename << "\n";
        return false;
    }
    // Очистка
    DriverNode* curr = head->next;
    while (curr) {
        DriverNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head->next = nullptr;
    idToDriverMap.clear();
    nameToIdMap.clear();

    std::string line;
    while (std::getline(file, line)) parseLine(line);
    file.close();
    return true;
}

// Разбор строки: "id \"fullName\" \"birthDate\" cityId"
void DriverTable::parseLine(const std::string& line) {
    if (line.empty()) return;
    istringstream iss(line);
    int id, cityId;
    string fullName, birthDate;
    iss >> id >> quoted(fullName) >> quoted(birthDate) >> cityId;
    addDriverNode(id, fullName, birthDate, cityId);
}

// Добавление узла в список и в хеш-таблицу
void DriverTable::addDriverNode(int id, const std::string& fullName,
    const std::string& birthDate, int cityId)
{
    DriverNode* newNode = new DriverNode(id, fullName, birthDate, cityId, head->next);
    head->next = newNode;
    idToDriverMap.insert(id, newNode);
    nameToIdMap[fullName] = id;
}

// Добавление водителя (OK)
void DriverTable::addDriver(const std::string& fullName,
    const std::string& birthDate, int cityId)
{
    if (!validateName(fullName))
        throw invalid_argument("Invalid characters in name");
    if (!validateDate(birthDate))
        throw invalid_argument("Incorrect date format");
    if (!validateAge(birthDate))
        throw invalid_argument("Driver must be between 18 and 100 years old");

    int newId = 1;
    for (auto& pair : nameToIdMap) {
        if (pair.second >= newId) newId = pair.second + 1;
    }
    addDriverNode(newId, fullName, birthDate, cityId);
}

// Удаление водителя по ID
void DriverTable::deleteDriverById(int id) {
    DriverNode* prev = head;
    DriverNode* curr = head->next;
    while (curr) {
        if (curr->id == id) {
            prev->next = curr->next;
            idToDriverMap.remove(id);
            nameToIdMap.erase(curr->fullName);
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Итератор: сброс на начало списка
void DriverTable::driverIteratorReset() const {
    currentIterator = head->next;
}

// Проверка наличия следующего
bool DriverTable::driverIteratorHasNext() const {
    return currentIterator != nullptr;
}

// Получение следующего
DriverTable::DriverInfo DriverTable::driverIteratorNext() const {
    DriverInfo info{};
    if (!currentIterator) return info;
    info.id = currentIterator->id;
    info.fullName = currentIterator->fullName;
    info.birthDate = currentIterator->birthDate;
    info.cityId = currentIterator->cityId;
    currentIterator = currentIterator->next;
    return info;
}

// Геттер: получить ФИО по ID
std::string DriverTable::getDriverNameById(int id) const {
    DriverNode* node = idToDriverMap.find<DriverNode>(id);
    return node ? node->fullName : "";
}

// Геттер: получить ID города для водителя
int DriverTable::getCityIdForDriver(const std::string& fullName) const {
    int id = getDriverId(fullName);
    if (id == -1) return -1;
    DriverNode* node = idToDriverMap.find<DriverNode>(id);
    return node ? node->cityId : -1;
}

// Геттер: получить ID по ФИО (в случае дублей возвращает первый попавшийся,
// если заданы birthDate и cityId — найдет точное совпадение)
int DriverTable::getDriverId(const std::string& fullName,
    const std::string& birthDate,
    int cityId) const
{
    std::vector<DriverInfo> candidates = findAllByName(fullName);
    if (candidates.empty()) return -1;
    if (candidates.size() == 1 && birthDate.empty() && cityId == -1) {
        return candidates[0].id;
    }
    // Если задана только дата рождения
    if (!birthDate.empty() && cityId == -1) {
        for (auto& d : candidates) {
            if (d.birthDate == birthDate) return d.id;
        }
        return -1;
    }
    // Если заданы дата и город
    if (!birthDate.empty() && cityId != -1) {
        for (auto& d : candidates) {
            if (d.birthDate == birthDate && d.cityId == cityId) return d.id;
        }
        return -1;
    }
    // Если заданы только город
    if (birthDate.empty() && cityId != -1) {
        for (auto& d : candidates) {
            if (d.cityId == cityId) return d.id;
        }
        return -1;
    }
    // Несколько совпадений, но неподходят уточнения
    return -1;
}

// Вспомогательное: вернуть всех водителей с данным ФИО
std::vector<DriverTable::DriverInfo> DriverTable::findAllByName(const std::string& fullName) const {
    std::vector<DriverInfo> result;
    DriverNode* curr = head->next;
    while (curr) {
        if (curr->fullName == fullName) {
            result.push_back(cloneInfo(curr));
        }
        curr = curr->next;
    }
    return result;
}

// Обновление ссылок при удалении города: устанавливаем cityId = -1
void DriverTable::updateCityReferences(int deletedCityId) {
    DriverNode* curr = head->next;
    while (curr) {
        if (curr->cityId == deletedCityId) {
            curr->cityId = -1;
        }
        curr = curr->next;
    }
}

// Редактирование ФИО
bool DriverTable::updateDriverName(int id, const std::string& newName) {
    if (!validateName(newName)) return false;
    DriverNode* node = idToDriverMap.find<DriverNode>(id);
    if (!node) return false;
    nameToIdMap.erase(node->fullName);
    node->fullName = newName;
    nameToIdMap[newName] = id;
    return true;
}

// Редактирование даты рождения
bool DriverTable::updateDriverBirthDate(int id, const std::string& newBirthDate) {
    if (!validateDate(newBirthDate) || !validateAge(newBirthDate)) return false;
    DriverNode* node = idToDriverMap.find<DriverNode>(id);
    if (!node) return false;
    node->birthDate = newBirthDate;
    return true;
}

// Редактирование города
bool DriverTable::updateDriverCity(int id, int newCityId) {
    DriverNode* node = idToDriverMap.find<DriverNode>(id);
    if (!node) return false;
    node->cityId = newCityId;
    return true;
}

// Проверка ФИО
bool DriverTable::validateName(const std::string& name) const {
    static const regex pattern("^[A-Za-z\\s]+$");
    return regex_match(name, pattern);
}

// Проверка формата даты
bool DriverTable::validateDate(const std::string& date) const {
    static const regex pattern("^\\d{2}\\.\\d{2}\\.\\d{4}$");
    return regex_match(date, pattern);
}

// Проверка возраста: 18–100
bool DriverTable::validateAge(const std::string& birthDate) const {
    int age = calculateAgeFromBirth(birthDate);
    return age >= 18 && age <= 100;
}

// Сохранение в файл
void DriverTable::saveToFile() const {
    ofstream file("drivers.txt");
    if (!file.is_open()) {
        cerr << "Error opening drivers file for writing!" << endl;
        return;
    }
    DriverNode* curr = head->next;
    while (curr) {
        file << curr->id << ' '
            << quoted(curr->fullName) << ' '
            << quoted(curr->birthDate) << ' '
            << curr->cityId << '\n';
        curr = curr->next;
    }
    file.close();
}

// Добавление фильтра
void DriverTable::addFilter(const std::string& field, int cmpType, const std::string& value) {
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

// Очистка всех фильтров
void DriverTable::clearFilters() {
    Filter* f = currentFilter;
    while (f) {
        Filter* nxt = f->next;
        delete f;
        f = nxt;
    }
    currentFilter = nullptr;
}

// Проверка одного поля на совпадение
bool DriverTable::matchField(const DriverNode* node, const std::string& field,
    int cmpType, const std::string& value) const
{
    if (field == "fullName") {
        if (cmpType == 1) return node->fullName.find(value) != string::npos;
        if (cmpType == 2) return node->fullName == value;
    }
    else if (field == "birthDate") {
        if (cmpType == 2) return node->birthDate == value;
    }
    return false;
}

// Клонирование данных узла
DriverTable::DriverInfo DriverTable::cloneInfo(const DriverNode* node) const {
    DriverInfo info;
    info.id = node->id;
    info.fullName = node->fullName;
    info.birthDate = node->birthDate;
    info.cityId = node->cityId;
    return info;
}

// Применение всех активных фильтров, возвращает динамический массив DriverInfo
DriverTable::DriverInfo* DriverTable::applyFilters(int& outCount) const {
    int count = 0;
    DriverNode* curr = head->next;
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
        if (match) count++;
        curr = curr->next;
    }
    if (count == 0) {
        outCount = 0;
        return nullptr;
    }
    DriverInfo* arr = new DriverInfo[count];
    int idx = 0;
    curr = head->next;
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
            arr[idx++] = cloneInfo(curr);
        }
        curr = curr->next;
    }
    outCount = count;
    return arr;
}

// Количество фильтров
int DriverTable::getFilterCount() const {
    int count = 0;
    Filter* f = currentFilter;
    while (f) {
        count++;
        f = f->next;
    }
    return count;
}

// Описание заданного фильтра
std::string DriverTable::getFilterDescription(int index) const {
    int i = 0;
    Filter* f = currentFilter;
    while (f) {
        if (i == index) {
            ostringstream oss;
            if (f->field == "fullName") {
                if (f->cmpType == 1) oss << "Full Name contains \"" << f->value << "\"";
                else oss << "Full Name equals \"" << f->value << "\"";
            }
            else if (f->field == "birthDate") {
                oss << "Birth Date = \"" << f->value << "\"";
            }
            else if (f->field == "birthDate") {
                if (f->cmpType == 3) oss << "Born before " << f->value;
                else if (f->cmpType == 4) oss << "Born after " << f->value;
            }
            return oss.str();
        }
        f = f->next;
        i++;
    }
    return "";
}

// Удалить конкретный фильтр по индексу
void DriverTable::removeFilterAt(int index) {
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