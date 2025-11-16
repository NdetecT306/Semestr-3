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
bool hasCsvExtension(const string& filename);
bool lockTable(const string& tableName);
bool unlockTable(const string& tableName);
bool isTableLocked(const string& tableName);
bool waitForTableUnlock(const string& tableName, int maxWaitMs);
int getNextPrimaryKey(const string& tableName);
bool createDirectory();
