#include "command.h"
#include "lock.h"
#include "MYCSV.h"
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
extern string currentSchemaName;
extern json schemaConfig;
extern string mySchema;
bool onlyWHERECondition(const string& condition, const Vec& tables, const Vec& currentRows) {// Простое сравнение с WHERe
    size_t EQUAL = condition.find('=');
    if (EQUAL == string::npos) return true;
    string leftSide = condition.substr(0, EQUAL);
    string rightSide = condition.substr(EQUAL + 1);
    leftSide.erase(0, leftSide.find_first_not_of(" "));
    leftSide.erase(leftSide.find_last_not_of(" ") + 1);
    rightSide.erase(0, rightSide.find_first_not_of(" "));
    rightSide.erase(rightSide.find_last_not_of(" ") + 1);
    size_t DOT = leftSide.find('.');
    if (DOT == string::npos) {
        DOT = rightSide.find('.');
        if (DOT == string::npos) return true;
        swap(leftSide, rightSide);
        DOT = leftSide.find('.');
    }
    string tableName = leftSide.substr(0, DOT);
    string columnName = leftSide.substr(DOT + 1);
    int tableInd = -1;
    for (size_t i = 0; i < tables.getSize(); i++) {
        if (tables[i] == tableName) {
            tableInd = i;
            break;
        }
    }
    if (tableInd == -1 || tableInd >= currentRows.getSize()) {
        return false;
    }
    int colInd = getColumnIndex(tableName, columnName);
    if (colInd == -1) return false;
    string rowData = currentRows[tableInd];
    Vec columns;
    stringstream ss(rowData);
    string field;
    while (getline(ss, field, ',')) {
        columns.PUSH(field);
    }
    if (colInd + 1 >= columns.getSize()) return false;
    string leftValue = columns[colInd + 1];
    string rightValue;
    if (rightSide[0] == '\'' && rightSide[rightSide.length()-1] == '\'') {
        rightValue = rightSide.substr(1, rightSide.length() - 2);
    } else if (rightSide.find('.') != string::npos) {
        size_t rightDOT = rightSide.find('.');
        string rightTable = rightSide.substr(0, rightDOT);
        string right_column = rightSide.substr(rightDOT + 1);
        int rightTableInd = -1;
        for (size_t i = 0; i < tables.getSize(); i++) {
            if (tables[i] == rightTable) {
                rightTableInd = i;
                break;
            }
        }
        if (rightTableInd == -1 || rightTableInd >= currentRows.getSize()) return false;
        int rightColInd = getColumnIndex(rightTable, right_column);
        if (rightColInd == -1) return false;
        string rightRow = currentRows[rightTableInd];
        Vec rightCol;
        stringstream rightss(rightRow);
        string rightField;
        while (getline(rightss, rightField, ',')) {
            rightCol.PUSH(rightField);
        }
        if (rightColInd + 1 >= rightCol.getSize()) return false;
        rightValue = rightCol[rightColInd + 1];
    } else {
        rightValue = rightSide;
    }
    return (leftValue == rightValue);
}

bool withANDandORCondition(string cond, const Vec& tables, const Vec& currentRows) {//AND и OR
    cond.erase(0, cond.find_first_not_of(" "));
    cond.erase(cond.find_last_not_of(" ") + 1);
    if (cond.empty()) return true;
    if (cond[0] == '(' && cond[cond.length()-1] == ')') {
        int depth = 1;
        bool is_outer = true;
        for (size_t i = 1; i < cond.length() - 1; i++) {
            if (cond[i] == '(') depth++;
            if (cond[i] == ')') depth--;
            if (depth == 0) {
                is_outer = false;
                break;
            }
        }
        if (is_outer && depth == 1) {
            return withANDandORCondition(cond.substr(1, cond.length() - 2), tables, currentRows);
        }
    }
    int parenDepth = 0;
    bool in_quotes = false;
    size_t orPos = string::npos;
    size_t andPos = string::npos;
    for (size_t i = 0; i < cond.length(); i++) {
        char c = cond[i];
        if (c == '\'') in_quotes = !in_quotes;
        if (!in_quotes) {
            if (c == '(') parenDepth++;
            else if (c == ')') parenDepth--;
            if (parenDepth == 0) {
                if (andPos == string::npos && i + 3 < cond.length() && cond.substr(i, 4) == "AND ") {
                    andPos = i;
                } else if (orPos == string::npos && i + 2 < cond.length() && 
                    cond.substr(i, 3) == "OR ") {
                    orPos = i;
                }
            }
        }
    }
    if (andPos != string::npos) {
        string leftPart = cond.substr(0, andPos);
        string rightPart = cond.substr(andPos + 4);
        leftPart.erase(0, leftPart.find_first_not_of(" "));
        leftPart.erase(leftPart.find_last_not_of(" ") + 1);
        rightPart.erase(0, rightPart.find_first_not_of(" "));
        rightPart.erase(rightPart.find_last_not_of(" ") + 1);
        bool leftRes = withANDandORCondition(leftPart, tables, currentRows);
        bool rightRes = withANDandORCondition(rightPart, tables, currentRows);
        return leftRes && rightRes;
    } else if (orPos != string::npos) {
        string leftPart = cond.substr(0, orPos);
        string rightPart = cond.substr(orPos + 3);
        leftPart.erase(0, leftPart.find_first_not_of(" "));
        leftPart.erase(leftPart.find_last_not_of(" ") + 1);
        rightPart.erase(0, rightPart.find_first_not_of(" "));
        rightPart.erase(rightPart.find_last_not_of(" ") + 1);
        bool leftRes = withANDandORCondition(leftPart, tables, currentRows);
        bool rightRes = withANDandORCondition(rightPart, tables, currentRows);
        return leftRes || rightRes;
    }
    return onlyWHERECondition(cond, tables, currentRows);
}
bool checkWhereCondition(const string& whereCond, const Vec& tables, const Vec& current_rows) { //Начало проверки WHERE
    if (whereCond.empty()) return true;
    return withANDandORCondition(whereCond, tables, current_rows);
}
//Наше декартово произведение
void processCombination(size_t tableIndex, Vec& currentCombination, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result) {
    if (tableIndex == tables.getSize()) {
        if (checkWhereCondition(whereCond, tables, currentCombination)) {
            string res_row;
            for (size_t colIdx = 0; colIdx < columns.getSize(); colIdx++) {
                if (colIdx > 0) res_row += ",";
                string fullCol = columns[colIdx];
                size_t DOT = fullCol.find('.');
                if (DOT == string::npos) {
                    res_row += "NULL";
                    continue;
                }
                string tableName = fullCol.substr(0, DOT);
                string colName = fullCol.substr(DOT + 1);
                int targetTableIndex = -1;
                for (size_t i = 0; i < tables.getSize(); i++) {
                    if (tables[i] == tableName) {
                        targetTableIndex = i;
                        break;
                    }
                }
                if (targetTableIndex == -1) {
                    res_row += "NULL";
                    continue;
                }
                int colIndex = getColumnIndex(tableName, colName);
                if (colIndex == -1) {
                    res_row += "NULL";
                    continue;
                }
                string rowData = currentCombination[targetTableIndex];
                Vec rowColumns;
                stringstream row_ss(rowData);
                string field;
                while (getline(row_ss, field, ',')) {
                    rowColumns.PUSH(field);
                }
                if (colIndex + 1 < rowColumns.getSize()) {
                    res_row += rowColumns[colIndex + 1];
                } else {
                    res_row += "NULL";
                }
            }
            result.PUSH(res_row);
        }
        return;
    }
    string currentTable = tables[tableIndex];
    Vec tableData = getTableDataFromHash(currentTable);
    for (size_t i = 0; i < tableData.getSize(); i++) {
        currentCombination.PUSH(tableData[i]);
        processCombination(tableIndex + 1, currentCombination, tables, columns, whereCond, result);
        currentCombination.erase(currentCombination.getSize() - 1);
    }
}
void SelectCommand(const string& input) { //SELECT
    size_t SELECT = input.find("SELECT");
    size_t FROM = input.find("FROM");
    size_t WHERE = input.find("WHERE");
    if (SELECT == string::npos || FROM == string::npos) {
        cerr << "Неверный формат запроса SELECT" << endl;
        return;
    }
    string columnsPart = input.substr(SELECT + 6, FROM - SELECT - 6);
    columnsPart.erase(0, columnsPart.find_first_not_of(" "));
    columnsPart.erase(columnsPart.find_last_not_of(" ") + 1);
    Vec columns;
    size_t pos = 0;
    while (pos < columnsPart.length()) {
        size_t COMMA = columnsPart.find(',', pos);
        if (COMMA == string::npos) {
            string col = columnsPart.substr(pos);
            col.erase(0, col.find_first_not_of(" "));
            col.erase(col.find_last_not_of(" ") + 1);
            if (!col.empty()) columns.PUSH(col);
            break;
        } else {
            string col = columnsPart.substr(pos, COMMA - pos);
            col.erase(0, col.find_first_not_of(" "));
            col.erase(col.find_last_not_of(" ") + 1);
            if (!col.empty()) columns.PUSH(col);
            pos = COMMA + 1;
        }
    }
    string tablesPart;
    if (WHERE != string::npos) {
        tablesPart = input.substr(FROM + 4, WHERE - FROM - 4);
    } else {
        tablesPart = input.substr(FROM + 4);
    }
    tablesPart.erase(0, tablesPart.find_first_not_of(" "));
    tablesPart.erase(tablesPart.find_last_not_of(" ") + 1);
    Vec tables;
    pos = 0;
    while (pos < tablesPart.length()) {
        size_t COMMA = tablesPart.find(',', pos);
        if (COMMA == string::npos) {
            string table = tablesPart.substr(pos);
            table.erase(0, table.find_first_not_of(" "));
            table.erase(table.find_last_not_of(" ") + 1);
            if (!table.empty()) tables.PUSH(table);
            break;
        } else {
            string table = tablesPart.substr(pos, COMMA - pos);
            table.erase(0, table.find_first_not_of(" "));
            table.erase(table.find_last_not_of(" ") + 1);
            if (!table.empty()) tables.PUSH(table);
            pos = COMMA + 1;
        }
    }
    string whereCond;
    if (WHERE != string::npos) {
        whereCond = input.substr(WHERE + 5);
        whereCond.erase(0, whereCond.find_first_not_of(" "));
        whereCond.erase(whereCond.find_last_not_of(" ") + 1);
    }
    for (size_t i = 0; i < tables.getSize(); i++) {
        if (GET_SCHEMA(tables[i]) == nullptr) {
            cerr << "Таблица '" << tables[i] << "' не найдена в SCHEMA_HASH" << endl;
            return;
        }
    }
    for (size_t i = 0; i < columns.getSize(); i++) {
        size_t DOT = columns[i].find('.');
        if (DOT == string::npos) {
            cerr << "Неверный формат колонки: " << columns[i] << endl;
            return;
        }
        string tableName = columns[i].substr(0, DOT);
        string columnName = columns[i].substr(DOT + 1);
        bool found = false;
        for (size_t j = 0; j < tables.getSize(); j++) {
            if (tables[j] == tableName) {
                found = true;
                break;
            }
        }
        if (!found) {
            cerr << "Таблица '" << tableName << "' не указана в FROM" << endl;
            return;
        }
        if (getColumnIndex(tableName, columnName) == -1) {
            cerr << "Колонка '" << columnName << "' не существует в таблице '" << tableName << "'" << endl;
            return;
        }
    }
    Vec result;
    cartesianProductFromHash(tables, columns, whereCond, result);
    cout << "Результат SELECT (" << result.getSize() << " строк):" << endl;
    for (size_t i = 0; i < columns.getSize(); i++) {
        if (i > 0) cout << ",";
        cout << columns[i];
    }
    cout << endl;
    for (size_t i = 0; i < result.getSize(); i++) {
        cout << result[i] << endl;
    }
}
void InsertCommand(const string& input) { //INSERT INTO
    size_t INTO = input.find("INTO");
    if (INTO == string::npos) {
        cout << "Не найдено ключевое слово INTO" << endl;
        return;
    }
    size_t name = input.find_first_not_of(" ", INTO + 4);
    if (name == string::npos) {
        cerr << "Не найдено имя таблицы" << endl;
        return;
    }
    size_t VALUES = input.find("VALUES", name);
    if (VALUES == string::npos) {
        cerr << "Не найдено ключевое слово VALUES" << endl;
        return;
    }
    string tableName = input.substr(name, VALUES - name);
    tableName.erase(remove(tableName.begin(), tableName.end(), ' '), tableName.end());
    size_t openBra = input.find('(', VALUES);
    if (openBra == string::npos) {
        cerr << "Не найдена открывающая скобка" << endl;
        return;
    }
    size_t closeBra = input.find(')', openBra + 1);
    if (closeBra == string::npos) {
        cerr << "Не найдена закрывающая скобка" << endl;
        return;
    }
    string valueStr = input.substr(openBra + 1, closeBra - openBra - 1);
    Vec element;
    size_t ourPos = 0;
    while (ourPos < valueStr.length()) {
        while (ourPos < valueStr.length() && valueStr[ourPos] == ' ') {
            ourPos++;
        }
        if (ourPos >= valueStr.length()) break;
        if (valueStr[ourPos] == '\'') {
            size_t endQuote = valueStr.find('\'', ourPos + 1);
            if (endQuote == string::npos) {
                cerr << "Ошибка: незакрытая кавычка" << endl;
                return;
            }
            string value = valueStr.substr(ourPos + 1, endQuote - ourPos - 1);
            element.PUSH(value);
            ourPos = endQuote + 1;
            if (ourPos < valueStr.length() && valueStr[ourPos] == ',') {
                ourPos++;
            }
        } else {
            size_t COMMA = valueStr.find(',', ourPos);
            if (COMMA == string::npos) {
                string value = valueStr.substr(ourPos);
                value.erase(0, value.find_first_not_of(' '));
                value.erase(value.find_last_not_of(' ') + 1);
                element.PUSH(value);
                break;
            } else {
                string value = valueStr.substr(ourPos, COMMA - ourPos);
                value.erase(0, value.find_first_not_of(' '));
                value.erase(value.find_last_not_of(' ') + 1);
                element.PUSH(value);
                ourPos = COMMA + 1;
            }
        }
    }
    int primaryKey = insertIntoCSV(tableName, element);
    if (primaryKey != -1) {
        string rowData = to_string(primaryKey);
        for (size_t i = 0; i < element.getSize(); i++) {
            rowData += "," + element[i];
        }
        Vec hashValue;
        hashValue.PUSH(rowData);
        DOUBLE_HASH* tableHash = GET_SCHEMA(tableName);
        if (tableHash != nullptr) {
            int ind = hash1(primaryKey);
            int step = hash2(primaryKey);
            int startInd = ind;
            int trials = 0;
            while (trials < HASH_SIZE) {
                if (tableHash[ind].getIsEmpty() || tableHash[ind].getIsDeleted()) {
                    tableHash[ind].init(primaryKey, hashValue);
                    cout << "Успешно добавлено." << endl;
                    return;
                }
                ind = (ind + step) % HASH_SIZE;
                trials++;
                if (ind == startInd) break;
            }
        }
        cout << "Данные добавлены в таблицу с PK: " << primaryKey << ", но возникли проблемы с SCHEMA_HASH." << endl;
    } else {
        cout << "Ошибка при добавлении данных в таблицу." << endl;
    }
}
void DeleteCommand(const string& input) { //DELETE FROM
    size_t FROM = input.find("FROM");
    if (FROM == string::npos) {
        cerr << "Не найдено ключевое слово FROM." << endl;
        return;
    }
    size_t name = input.find_first_not_of(" ", FROM + 4);
    if (name == string::npos) {
        cerr << "Не найдено имя таблицы." << endl;
        return;
    }
    size_t WHERE = input.find("WHERE", name);
    if (WHERE == string::npos) {
        cerr << "Не найдено ключевое слово WHERE." << endl;
        return;
    }
    string tableName = input.substr(name, WHERE - name);
    tableName.erase(remove(tableName.begin(), tableName.end(), ' '), tableName.end());
    string cond = input.substr(WHERE + 5);
    size_t DOT = cond.find('.');
    if (DOT == string::npos) {
        cerr << "Не найден символ '.' в команде." << endl;
        return;
    }
    size_t EQUAL = cond.find('=', DOT);
    if (EQUAL == string::npos) {
        cerr << "Не найден символ '=' в команде." << endl;
        return;
    }
    string condTableName = cond.substr(0, DOT);
    condTableName.erase(remove(condTableName.begin(), condTableName.end(), ' '), condTableName.end());
    if (condTableName != tableName) {
        cerr << "Имя таблицы в условии '" << condTableName << "' не совпадает с указанной таблицей '" << tableName << "'" << endl;
        return;
    }
    string columnName = cond.substr(DOT + 1, EQUAL - DOT - 1);
    columnName.erase(remove(columnName.begin(), columnName.end(), ' '), columnName.end()); 
    size_t openKav = cond.find('\'', EQUAL);
    if (openKav == string::npos) {
        cerr << "Не найдена открывающая кавычка" << endl;
        return;
    }
    size_t closeKav = cond.find('\'', openKav + 1);
    if (closeKav == string::npos) {
        cerr << "Незакрытая кавычка" << endl;
        return;
    }
    string value = cond.substr(openKav + 1, closeKav - openKav - 1);
    Vec deletedPrimaryKeys;
    bool fileDeleted = deleteFromCSV(tableName, columnName, value, deletedPrimaryKeys);
    bool hashDeleted = false;
    DOUBLE_HASH* tableHash = GET_SCHEMA(tableName);
    if (tableHash != nullptr) {
        for (size_t i = 0; i < deletedPrimaryKeys.getSize(); i++) {
            try {
                int primaryKey = stoi(deletedPrimaryKeys[i]);
                for (int j = 0; j < HASH_SIZE; j++) {
                    if (tableHash[j].isValid() && tableHash[j].getKey() == primaryKey) {
                        tableHash[j].setIsDeleted(true);
                        tableHash[j].setIsEmpty(true);
                        hashDeleted = true;
                        break;
                    }
                }
            } catch (const exception& e) {
                cerr << "Ошибка преобразования первичного ключа: " << deletedPrimaryKeys[i] << endl;
            }
        }
    }
    if (fileDeleted || hashDeleted) {
        cout << "Удаление прошло успешно. Удалено записей: " << deletedPrimaryKeys.getSize() << endl;
    } else {
        cout << "Строки для удаления не найдены." << endl;
    }
}
