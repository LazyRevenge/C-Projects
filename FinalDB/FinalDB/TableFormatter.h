#pragma once
#include <string>
#include <vector>
#include <sstream>

class TableFormatter {
public:
    static std::string format(const std::vector<std::vector<std::string>>& rows);
};