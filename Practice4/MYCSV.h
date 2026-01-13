#ifndef MYCSV_H
#define MYCSV_H
#include "monitor.h"
#include "struct.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include "json.hpp"
#include "lock.h"
using namespace std;

bool hasCsvExtension(const string& filename);
int getNextPrimaryKey(const string& tableName);
bool createTableDirectory(const string& tableName);
bool createDirectory();
Vec getTableDataFromHash(const string& tableName);
int getColumnIndex(const string& tableName, const string& columnName);
void loadAllDataToHash();
int getCurrentRowCount(const string& csvFilePath);
int getTuplesLimit();
bool checkForeignKeyExists(const string& tableName, const string& columnName, const string& value);
bool checkForeignKeysForInsert(const string& tableName, const Vec& values);
bool checkForeignKeyForDelete(const string& tableName, const string& columnName, const string& value);
string insertIntoCSV(const string& tableName, Vec& values);
bool deleteFromCSV(const string& tableName, const string& columnName, const string& value, Vec& deletedPrimaryKeys);
void cartesianProductFromHash(const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);

#endif

