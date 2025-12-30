#include <iostream>
#include <string>
#include "json.hpp"
#include <sstream>
#include <algorithm>
#include "MYCSV.h"
using namespace std;

void SelectCommand(const string& input);
void InsertCommand(const string& input);
void DeleteCommand(const string& input);
bool onlyWHERECondition(const string& condition, const Vec& tables, const Vec& currentRows);
bool withANDandORCondition(string cond, const Vec& tables, const Vec& currentRows);
bool checkWhereCondition(const string& whereCond, const Vec& tables, const Vec& current_rows);
void processCombination(size_t tableIndex, Vec& currentCombination, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);
