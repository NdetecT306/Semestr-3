#include "commands.h"
#include "struct.h"
#include "CSV.h"
#include "Block.h"
#include "struct.h"
#include "workWithJson.h"
Vec getTableColumns(const string& table_name) {//Получение списка колонок таблицы
    Vec columns;
    string path = currentSchemaName + "/" + table_name + "/0.csv";
    ifstream file(path);
    if (!file.is_open()) {
        return columns;
    }
    string header;
    if (getline(file, header)) {
        stringstream ss(header);
        string column;
        while (getline(ss, column, ',')) {
            columns.PUSH(column);
        }
    }
    file.close();
    return columns;
}
Vec readTable(const string& table_name) {// Чтение данных таблицы
    Vec result;
    string path = currentSchemaName + "/" + table_name;
    if (!fs::exists(path)) {
        return result;
    }
    Vec files;// Список CSV файлов
    for (const auto& el : fs::directory_iterator(path)) {
        if (el.is_regular_file()) {
            string filename = el.path().filename().string();
            if (hasCsvExtension(filename)) {
                files.PUSH(el.path().string());
            }
        }
    }
    for (size_t i = 0; i < files.getSize(); i++) {
        for (size_t j = i + 1; j < files.getSize(); j++) {
            string file1 = fs::path(files[i]).filename().string();
            string file2 = fs::path(files[j]).filename().string();
            int num1 = stoi(file1.substr(0, file1.find('.')));
            int num2 = stoi(file2.substr(0, file2.find('.')));
            if (num1 > num2) {
                string vrem = files[i];
                files[i] = files[j];
                files[j] = vrem;
            }
        }
    }
    for (size_t i = 0; i < files.getSize(); i++) {// Читаем данные из файлов
        ifstream file(files[i]);
        if (!file.is_open()) {
            continue;
        }
        string line;
        bool first = true;
        while (getline(file, line)) {
            if (first) {
                first = false;
                continue; 
            }
            if (!line.empty()) {
                result.PUSH(line);
            }
        }
        file.close();
    }
    return result;
}
bool checkSimpleCondition(const string& condition, const Vec& tables, const Vec& currentRows) { //Только WHERE
    size_t EQUAL = condition.find('=');
    if (EQUAL == string::npos) {
        return true;
    }
    string leftSide = condition.substr(0, EQUAL);
    string rightSide = condition.substr(EQUAL + 1);
    leftSide.erase(0, leftSide.find_first_not_of(" "));// Убираем пробелы
    leftSide.erase(leftSide.find_last_not_of(" ") + 1);
    rightSide.erase(0, rightSide.find_first_not_of(" "));
    rightSide.erase(rightSide.find_last_not_of(" ") + 1);
    size_t DOT = leftSide.find('.');
    if (DOT == string::npos) {
        return true;
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
    int colInd = getColumnIndex(tableName, columnName);// Получаем индекс колонки
    if (colInd == -1) {
        return false;
    }
    string rows = currentRows[tableInd];// Парсим строку таблицы
    Vec columns;
    stringstream ss(rows);
    string field;
    while (getline(ss, field, ',')) {
        columns.PUSH(field);
    }
    if (colInd + 1 >= columns.getSize()) {
        return false;
    }
    string leftValue = columns[colInd + 1];
    string rightValue;
    if (rightSide[0] == '\'' && rightSide[rightSide.length()-1] == '\'') {// Обрабатываем правую часть
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
        if (rightTableInd == -1 || rightTableInd >= currentRows.getSize()) {
            return false;
        }
        int rightColInd = getColumnIndex(rightTable, right_column);
        if (rightColInd == -1) {
            return false;
        }
        string rightRow = currentRows[rightTableInd];// Парсим правую строку таблицы
        Vec rightCol;
        stringstream rightss(rightRow);
        string rightField;
        while (getline(rightss, rightField, ',')) {
            rightCol.PUSH(rightField);
        }
        if (rightColInd + 1 >= rightCol.getSize()) {
            return false;
        }
        rightValue = rightCol[rightColInd + 1];
    } else {
        rightValue = rightSide;
    }
    return (leftValue == rightValue);
}
bool parseCondition(string cond, const Vec& tables, const Vec& currentRows) {
    cond.erase(0, cond.find_first_not_of(" "));
    cond.erase(cond.find_last_not_of(" ") + 1);
    bool InQ = false;
    int paren_depth = 0;
    size_t OR = string::npos;
    size_t AND = string::npos;
    for (size_t i = 0; i < cond.length(); i++) {
        if (cond[i] == '\'') InQ = !InQ;
        if (cond[i] == '(') paren_depth++;
        if (cond[i] == ')') paren_depth--;
        if (!InQ && paren_depth == 0) {
            if (OR == string::npos && i + 3 <= cond.length() && cond.substr(i, 3) == "OR ") {
                OR = i;
                i += 2;
            }
            else if (AND == string::npos && i + 4 <= cond.length() && cond.substr(i, 4) == "AND ") {
                AND = i;
                i += 3;
            }
        }
    }
    if (OR != string::npos) {
        string leftPart = cond.substr(0, OR);
        string rightPart = cond.substr(OR + 3);
        leftPart.erase(0, leftPart.find_first_not_of(" "));
        leftPart.erase(leftPart.find_last_not_of(" ") + 1);
        rightPart.erase(0, rightPart.find_first_not_of(" "));
        rightPart.erase(rightPart.find_last_not_of(" ") + 1);
        return parseCondition(leftPart, tables, currentRows) || parseCondition(rightPart, tables, currentRows);
    }
    if (AND != string::npos) {
        string leftPart = cond.substr(0, AND);
        string rightPart = cond.substr(AND + 4);
        leftPart.erase(0, leftPart.find_first_not_of(" "));
        leftPart.erase(leftPart.find_last_not_of(" ") + 1);
        rightPart.erase(0, rightPart.find_first_not_of(" "));
        rightPart.erase(rightPart.find_last_not_of(" ") + 1);
        return parseCondition(leftPart, tables, currentRows) && parseCondition(rightPart, tables, currentRows);
    }
    if (cond.length() >= 2 && cond[0] == '(' && cond[cond.length()-1] == ')') {
        int depth = 0;
        bool is_outer = true;
        for (size_t i = 0; i < cond.length(); i++) {
            if (cond[i] == '(') depth++;
            if (cond[i] == ')') depth--;
            if (depth == 0 && i < cond.length() - 1) {
                is_outer = false;
                break;
            }
        }
        if (is_outer) {
            return parseCondition(cond.substr(1, cond.length() - 2), tables, currentRows);
        }
    }
    return checkSimpleCondition(cond, tables, currentRows);// Если нет операторов и скобок, проверяем как простое условие
}
bool checkWhereCondition(const string& whereCond, const Vec& tables, const Vec& current_rows) {
    if (whereCond.empty()) return true;
    return parseCondition(whereCond, tables, current_rows);
}
void simpleCartesianProduct(const Vec& all_data, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result) { //Декартово
    int count = 0;
    if (tables.getSize() == 1) {
        Vec table_data;
        string data_str = all_data[0];
        stringstream ss(data_str);
        string line;
        while (getline(ss, line)) {
            if (!line.empty()) {
                table_data.PUSH(line);
            }
        }
        for (size_t i = 0; i < table_data.getSize(); i++) {
            Vec current_row;
            current_row.PUSH(table_data[i]);
            if (checkWhereCondition(whereCond, tables, current_row)) {
                string res;
                for (size_t colIdx = 0; colIdx < columns.getSize(); colIdx++) {
                    if (colIdx > 0) res += ",";
                    string fullCol = columns[colIdx];
                    size_t DOT = fullCol.find('.');
                    string tableName = fullCol.substr(0, DOT);
                    string colName = fullCol.substr(DOT + 1);
                    int colIndex = getColumnIndex(tableName, colName);
                    if (colIndex != -1) {
                        Vec rowColumns;// Парсим строку чтобы получить значение нужной колонки
                        string rowData = table_data[i];
                        stringstream row_ss(rowData);
                        string field;
                        while (getline(row_ss, field, ',')) {
                            rowColumns.PUSH(field);
                        }
                        if (colIndex + 1 < rowColumns.getSize()) {
                            res += rowColumns[colIndex + 1];
                        } else {
                            res += "NULL";
                        }
                    } else {
                        res += "NULL";
                    }
                }
                result.PUSH(res);
                count++;
            }
        }
    }
}
void SelectCommand(const string& input) {
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
    for (size_t i = 0; i < tables.getSize(); i++) {// Проверяем существование таблиц
        string tablePath = currentSchemaName + "/" + tables[i];
        if (!fs::exists(tablePath)) {
            cerr << "Таблица '" << tables[i] << "' не существует" << endl;
            return;
        }
    }
    for (size_t i = 0; i < columns.getSize(); i++) {// Проверяем корректность колонок
        size_t DOT = columns[i].find('.');
        if (DOT == string::npos) {
            cerr << "Неверный формат колонки: " << columns[i] << endl;
            return;
        }
        string tableName = columns[i].substr(0, DOT);
        string columnName = columns[i].substr(DOT + 1);
        bool found = false;// Проверяем, что таблица есть в FROM
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
    Vec allTableData;
    for (size_t i = 0; i < tables.getSize(); i++) {
        Vec table_data = readTable(tables[i]);
        string table_data_str;
        for (size_t j = 0; j < table_data.getSize(); j++) {
            if (j > 0) table_data_str += "\n";
            table_data_str += table_data[j];
        }
        allTableData.PUSH(table_data_str);
    }
    Vec result;
    simpleCartesianProduct(allTableData, tables, columns, whereCond, result);
    cout << "Результат SELECT (" << result.getSize() << " строк):" << endl;
    for (size_t i = 0; i < columns.getSize(); i++) {// Заголовки
        if (i > 0) cout << ",";
        cout << columns[i];
    }
    cout << endl;
    for (size_t i = 0; i < result.getSize(); i++) {
        cout << result[i] << endl;
    }
}
void InsertCommand(const string& input) {//INSERT INTO таблица1 VALUES ('somedata', '12345')
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
    int tableKey = getKeyByTableName(tableName);
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
        } 
        else {
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
    bool hashSuccess = true;
    for (size_t i = 0; i < element.getSize(); i++) {
        if (!ADD(tableKey, element[i])) {
            cerr << "Ошибка при добавлении значения '" << element[i] << "' в хэш-таблицу" << endl;
            hashSuccess = false;
        }
    }
    bool fileSuccess = insertIntoCSV(tableName, element);
    if (hashSuccess && fileSuccess) {
        cout << "Успешно добавлено в таблицу." << endl;
    } else {
        cout << "Что-то пошло не так." << endl;
    }
}
void DeleteCommand(const string& input) { //DELETE FROM таблица1 WHERE таблица1.колонка1 = '123'
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
    int tableKey = getKeyByTableName(tableName);
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
    bool hashDeleted = SETDEL(tableKey, value);
    bool fileDeleted = deleteFromCSV(tableName, columnName, value);
    if (hashDeleted || fileDeleted) {
        cout << "Удаление прошло успешно." << endl;
    } else {
        cout << "Строки для удаления не найдены." << endl;
    }
}
