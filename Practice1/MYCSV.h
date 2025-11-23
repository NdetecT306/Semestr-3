#ifndef CSV_H
#define CSV_H
#include <string>
#include "struct.h"
using namespace std;
extern string currentSchemaName;
extern json schemaConfig;
bool hasCsvExtension(const string& filename);
int getNextPrimaryKey(const string& tableName);
bool createTableDirectory(const string& tableName);
bool createDirectory();
Vec getTableDataFromHash(const string& tableName);
int getColumnIndex(const string& tableName, const string& columnName);
void loadAllDataToHash();
int getCurrentRowCount(const string& csvFilePath);
int getTuplesLimit();
int insertIntoCSV(const string& tableName, Vec& values);
bool deleteFromCSV(const string& tableName, const string& columnName, const string& value, Vec& deletedPrimaryKeys);
void processCombination(size_t tableIndex, Vec& currentCombination, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);
void cartesianProductFromHash(const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);
#endif
