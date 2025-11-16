#include <iostream> //библиотеки
#include <string>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <thread>
#include <functional>
#include <random>
#include "json.hpp"
#include "struct.h"
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace std;
namespace fs = filesystem;
bool loadSchema();
bool saveSchema();
int getKeyByTableName(const string& tableName);
struct TableData;
TableData readTableData(const string& tableName);
int getColumnIndex(const string& tableName, const string& columnName);
Vec parseCSVLine(const string& line);
