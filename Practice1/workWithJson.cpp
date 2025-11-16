#include "commands.h"
#include "struct.h"
#include "CSV.h"
#include "Block.h"
#include "struct.h"
#include "workWithJson.h"
bool loadSchema() { //Основа schema.json
    ifstream file(mySchema);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << mySchema << endl;
        return false;
    }
    file >> schemaConfig;
    currentSchemaName = schemaConfig["name"];
    tuplesLimit = schemaConfig["tuples_limit"];
    cout << "Схема '" << currentSchemaName << "' успешно загружена" << endl;
    return true;
}
bool saveSchema() { //Сохранить схему 
    ofstream file(mySchema);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << mySchema << " для записи" << endl;
        return false;
    }
    try {
        file << schemaConfig.dump(4);
        return true;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return false;
    }
}
int getKeyByTableName(const string& tableName) { //Рандомная генерация ключа хэша
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000); // диапазон от 1 до 1,000,000
    if (tableKeys.find(tableName) != tableKeys.end()) {
        return tableKeys[tableName];
    }
    int newKey = dis(gen);
    tableKeys[tableName] = newKey;
    return newKey;
}
struct TableData {// Структура для хранения данных таблицы
    string tableName;
    Vec data; 
};
TableData readTableData(const string& tableName) {
    TableData result;
    result.tableName = tableName;
    string tablePath = currentSchemaName + "/" + tableName;
    if (!fs::exists(tablePath)) {
        cerr << "Таблица '" << tableName << "' не существует" << endl;
        return result;
    }
    Vec csvFiles;// Читаем все CSV файлы таблицы
    for (const auto& entry : fs::directory_iterator(tablePath)) {
        if (entry.is_regular_file() && hasCsvExtension(entry.path().string())) {
            csvFiles.PUSH(entry.path().string());
        }
    }
    csvFiles.sort();
    for (size_t i = 0; i < csvFiles.getSize(); i++) {
        const auto& csvFilePath = csvFiles[i];
        ifstream file(csvFilePath);
        if (file.is_open()) {
            string line;
            bool isFirstLine = true;
            while (getline(file, line)) {
                if (isFirstLine) {
                    isFirstLine = false;
                    continue; // Пропускаем заголовок
                }
                if (!line.empty()) {
                    result.data.PUSH(line);
                }
            }
            file.close();
        }
    }
    return result;
}
int getColumnIndex(const string& tableName, const string& columnName) { //Получить номер колонки для Select
    if (!schemaConfig["structure"].contains(tableName)) {
        cerr << "Таблица '" << tableName << "' не найдена в схеме" << endl;
        return -1;
    }
    auto columns = schemaConfig["structure"][tableName];
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i] == columnName) {
            return i; 
        }
    }
    cerr << "Колонка '" << columnName << "' не найдена в таблице '" << tableName << "'" << endl;
    return -1;
}
Vec parseCSVLine(const string& line) {
    Vec result;
    string element;
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.PUSH(element);
            element.clear();
        } else {
            element += c;
        }
    }
    result.PUSH(element);
    return result;
}
