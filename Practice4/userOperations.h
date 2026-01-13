#ifndef USER_OPERATIONS_H
#define USER_OPERATIONS_H

#include <string>
#include "struct.h"
#include "MYCSV.h"
#include "balanceOperations.h"
#include "trading.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "json.hpp"
using namespace std;

string generateUserKey();
string getUserIdByUsername(const string& username);
string getUserIdByKey(const string& userKey);
bool initializeUserBalance(const string& userId);
#endif
