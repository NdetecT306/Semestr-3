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
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace std;
namespace fs = filesystem;
#include "struct.h"
int getNextFileNumber(const string& tablePath);
int getCurrentRowCount(const string& csvFilePath);
bool insertIntoCSV(const string& tableName, Vec& values);
bool deleteFromCSV(const string& tableName, const string& columnName, const string& value);
