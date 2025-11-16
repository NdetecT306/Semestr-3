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
void SelectCommand(const string& input);
void InsertCommand(const string& input);
void DeleteCommand(const string& input);
