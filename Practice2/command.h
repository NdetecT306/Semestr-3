#ifndef SQL_COMMANDS_H
#define SQL_COMMANDS_H
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "struct.h"
using namespace std;
bool onlyWHERECondition(const string& condition, const Vec& tables, const Vec& currentRows);
bool withANDandORCondition(string cond, const Vec& tables, const Vec& currentRows);
bool checkWhereCondition(const string& whereCond, const Vec& tables, const Vec& current_rows);
void processCombination(size_t tableIndex, Vec& currentCombination, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);
void SelectCommand(const string& input);
void InsertCommand(const string& input);
void DeleteCommand(const string& input);
#endif 
