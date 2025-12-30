#include "trading.h"
using json = nlohmann::json;
extern string currentSchemaName;
vector<Order> getOpenOrdersFromHash() {
    vector<Order> orders;
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        cout << "Таблица order не найдена в SCHEMA_HASH" << endl;
        return orders;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 6) {
                try {
                    Order order;
                    order.order_id = stoi(rowValues[0]);
                    order.user_id = stoi(rowValues[1]);
                    order.pair_id = stoi(rowValues[2]);
                    order.quantity = stof(rowValues[3]);
                    order.price = stof(rowValues[4]);
                    order.type = rowValues[5];
                    if (rowValues.getSize() >= 7) {
                        order.closed = rowValues[6];
                    } else {
                        order.closed = "";
                    }
                    if (!order.closed.empty()) {
                        continue;
                    }
                    orders.push_back(order);
                } catch (...) {
                    continue;
                }
            }
        }
    }
    return orders;
}
Order* findMatchingOrder(Order& newOrder) {
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        return nullptr;
    }
    Order* bestMatchingOrder = nullptr;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 6) {
                try {
                    Order existingOrder;
                    existingOrder.order_id = stoi(rowValues[0]);
                    existingOrder.user_id = stoi(rowValues[1]);
                    existingOrder.pair_id = stoi(rowValues[2]);
                    existingOrder.quantity = stof(rowValues[3]);
                    existingOrder.price = stof(rowValues[4]);
                    existingOrder.type = rowValues[5];
                    if (rowValues.getSize() >= 7) {
                        existingOrder.closed = rowValues[6];
                    } else {
                        existingOrder.closed = "";
                    }
                    // Пропускаем закрытые ордера
                    if (!existingOrder.closed.empty()) {
                        continue;
                    }
                    // Пропускаем ордера того же пользователя
                    if (existingOrder.user_id == newOrder.user_id) {
                        continue;
                    }
                    // Проверяем ту же пару
                    if (existingOrder.pair_id != newOrder.pair_id) {
                        continue;
                    }
                    // Проверяем встречные типы и возможность сделки
                    bool canTrade = false;
                    float executionPrice = 0.0f;
                    if (newOrder.type == "buy" && existingOrder.type == "sell") {
                        // Цена продавца <= цена покупателя
                        canTrade = (existingOrder.price <= newOrder.price);
                        executionPrice = existingOrder.price; 
                    } else if (newOrder.type == "sell" && existingOrder.type == "buy") {
                        canTrade = (newOrder.price <= existingOrder.price);
                        executionPrice = newOrder.price;
                    } else {
                        continue;
                    }
                    if (!canTrade) {
                        continue;
                    }
                    if (bestMatchingOrder == nullptr) {
                        bestMatchingOrder = new Order(existingOrder);
                        bestMatchingOrder->execution_price = executionPrice;
                    } else {
                        if (executionPrice < bestMatchingOrder->execution_price) {
                            delete bestMatchingOrder;
                            bestMatchingOrder = new Order(existingOrder);
                            bestMatchingOrder->execution_price = executionPrice;
                        }
                    }
                } catch (const exception& e) {
                    cout << "Ошибка парсинга: " << e.what() << endl;
                    continue;
                }
            }
        }
    }
    return bestMatchingOrder;
}
// Обновление количества в ордере для его частичного исполнения
bool updateOrderQuantity(int order_id, float new_quantity) {
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) return false;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            
            if (rowValues.getSize() >= 7) {
                int current_order_id = stoi(rowValues[0]);
                if (current_order_id == order_id) {
                    // Обновляем количество
                    rowValues[3] = to_string(new_quantity);
                    // Собираем обратно строку
                    string updatedRow;
                    for (size_t j = 0; j < rowValues.getSize(); j++) {
                        if (j > 0) updatedRow += ",";
                        updatedRow += rowValues[j];
                    }
                    // Обновляем в хэше
                    Vec updatedValue;
                    updatedValue.PUSH(updatedRow);
                    orderHash[i].setValue(updatedValue);
                    // Обновляем в CSV
                    string tablePath = currentSchemaName + "/order";
                    for (const auto& entry : filesystem::directory_iterator(tablePath)) {
                        string filename = entry.path().filename().string();
                        if (entry.is_regular_file() && 
                            filename.length() >= 4 && 
                            filename.substr(filename.length() - 4) == ".csv") {
                            string csvFilePath = entry.path().string();
                            string tempFilePath = csvFilePath + ".tmp";
                            ifstream inputFile(csvFilePath);
                            ofstream outputFile(tempFilePath);
                            if (!inputFile.is_open() || !outputFile.is_open()) continue;
                            string line;
                            bool firstLine = true;
                            while (getline(inputFile, line)) {
                                if (firstLine) {
                                    outputFile << line << endl;
                                    firstLine = false;
                                    continue;
                                }
                                if (line.find(to_string(order_id) + ",") == 0) {
                                    outputFile << updatedRow << endl;
                                } else {
                                    outputFile << line << endl;
                                }
                            }
                            inputFile.close();
                            outputFile.close();
                            filesystem::remove(csvFilePath);
                            filesystem::rename(tempFilePath, csvFilePath);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
// Обновление баланса пользователя
bool updateUserBalance(int user_id, int lot_id, float amount) {
    DOUBLE_HASH* userLotHash = GET_SCHEMA("user_lot");
    if (userLotHash == nullptr) {
        return false;
    }
    bool found = false;
    int found_index = -1;
    // Сначала ищем существующую запись
    for (int i = 0; i < HASH_SIZE; i++) {
        if (userLotHash[i].isValid()) {
            Vec rowValues;
            string rowData = userLotHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() == 3) {
                int current_user_id = stoi(rowValues[0]);
                int current_lot_id = stoi(rowValues[1]);
                
                if (current_user_id == user_id && current_lot_id == lot_id) {
                    found = true;
                    found_index = i;
                    break;
                }
            }
        }
    }
    if (found) {
        // Обновляем существующую запись
        Vec rowValues;
        string rowData = userLotHash[found_index].getValue()[0];
        stringstream ss(rowData);
        string cell;
        while (getline(ss, cell, ',')) {
            rowValues.PUSH(cell);
        }
        if (rowValues.getSize() == 3) {
            float current_quantity = stof(rowValues[2]);
            float new_quantity = current_quantity + amount;
            // Обновляем в хэше
            string newRow = to_string(user_id) + "," + to_string(lot_id) + "," + to_string(new_quantity);
            Vec newValue;
            newValue.PUSH(newRow);
            userLotHash[found_index].setValue(newValue);
            // Обновляем в CSV файле
            string tablePath = currentSchemaName + "/user_lot";
            for (const auto& entry : filesystem::directory_iterator(tablePath)) {
                string filename = entry.path().filename().string();
                if (entry.is_regular_file() && 
                    filename.length() >= 4 && 
                    filename.substr(filename.length() - 4) == ".csv") {
                    string csvFilePath = entry.path().string();
                    string tempFilePath = csvFilePath + ".tmp";
                    ifstream inputFile(csvFilePath);
                    ofstream outputFile(tempFilePath);
                    if (!inputFile.is_open() || !outputFile.is_open()) continue;
                    string line;
                    bool firstLine = true;
                    while (getline(inputFile, line)) {
                        if (firstLine) {
                            outputFile << line << endl;
                            firstLine = false;
                            continue;
                        }
                        if (line.find(to_string(user_id) + "," + to_string(lot_id) + ",") == 0) {
                            outputFile << newRow << endl;
                        } else {
                            outputFile << line << endl;
                        }
                    }
                    inputFile.close();
                    outputFile.close();
                    filesystem::remove(csvFilePath);
                    filesystem::rename(tempFilePath, csvFilePath);
                    return true;
                }
            }
        }
    } else if (amount > 0) {
        // Создаем новую запись
        Vec balanceValues;
        balanceValues.PUSH(to_string(user_id));
        balanceValues.PUSH(to_string(lot_id));
        balanceValues.PUSH(to_string(amount));
        string result = insertIntoCSV("user_lot", balanceValues);
        return (result != "-1" && !result.empty());
    }
    return false;
}
float getUserBalance(int user_id, int lot_id) {
    DOUBLE_HASH* userLotHash = GET_SCHEMA("user_lot");
    if (userLotHash == nullptr) {
        return 0.0f;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (userLotHash[i].isValid()) {
            Vec rowValues;
            string rowData = userLotHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() == 3) {
                int current_user_id = stoi(rowValues[0]);
                int current_lot_id = stoi(rowValues[1]);
                if (current_user_id == user_id && current_lot_id == lot_id) {
                    try {
                        return stof(rowValues[2]);
                    } catch (...) {
                        return 0.0f;
                    }
                }
            }
        }
    }
    return 0.0f;
}
// Получение открытых ордеров пользователя
vector<Order> getUserOpenOrders(int user_id) {
    vector<Order> userOrders;
    vector<Order> allOrders = getOpenOrdersFromHash();
    for (const auto& order : allOrders) {
        if (order.user_id == user_id && order.closed.empty()) {
            userOrders.push_back(order);
        }
    }
    return userOrders;
}
// Закрытие ордера
bool closeOrder(int order_id, const string& timestamp) {
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        return false;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 1) {
                int current_order_id = stoi(rowValues[0]);
                if (current_order_id == order_id) {
                    // Обновляем в хэше
                    if (rowValues.getSize() >= 7) {
                        rowValues[6] = timestamp;
                    } else if (rowValues.getSize() == 6) {
                        // Добавляем поле closed если его нет
                        rowValues.PUSH(timestamp);
                    }
                    Vec updatedValue;
                    string updatedRow;
                    for (size_t j = 0; j < rowValues.getSize(); j++) {
                        if (j > 0) updatedRow += ",";
                        updatedRow += rowValues[j];
                    }
                    updatedValue.PUSH(updatedRow);
                    orderHash[i].setValue(updatedValue);
                    // Обновляем в CSV
                    string tablePath = currentSchemaName + "/order";
                    for (const auto& entry : filesystem::directory_iterator(tablePath)) {
                        string filename = entry.path().filename().string();
                        if (entry.is_regular_file() && 
                            filename.length() >= 4 && 
                            filename.substr(filename.length() - 4) == ".csv") {
                            string csvFilePath = entry.path().string();
                            string tempFilePath = csvFilePath + ".tmp";
                            ifstream inputFile(csvFilePath);
                            ofstream outputFile(tempFilePath);
                            if (!inputFile.is_open() || !outputFile.is_open()) continue;
                            string line;
                            bool firstLine = true;
                            while (getline(inputFile, line)) {
                                if (firstLine) {
                                    outputFile << line << endl;
                                    firstLine = false;
                                    continue;
                                }
                                if (line.find(to_string(order_id) + ",") == 0) {
                                    Vec columns;
                                    stringstream line_ss(line);
                                    string column;
                                    while (getline(line_ss, column, ',')) {
                                        columns.PUSH(column);
                                    }
                                    if (columns.getSize() >= 7) {
                                        columns[6] = timestamp;
                                    } else if (columns.getSize() == 6) {
                                        columns.PUSH(timestamp);
                                    }
                                    string newLine;
                                    for (size_t j = 0; j < columns.getSize(); j++) {
                                        if (j > 0) newLine += ",";
                                        newLine += columns[j];
                                    }
                                    outputFile << newLine << endl;
                                } else {
                                    outputFile << line << endl;
                                }
                            }
                            inputFile.close();
                            outputFile.close();
                            filesystem::remove(csvFilePath);
                            filesystem::rename(tempFilePath, csvFilePath);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool updateOrderQuantityAndClose(int order_id, float new_quantity, const string& timestamp) {
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        cout << "Таблица order не найдена" << endl;
        return false;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 1) {
                int current_order_id = stoi(rowValues[0]);
                if (current_order_id == order_id) {
                    if (rowValues.getSize() >= 7) {
                        rowValues[3] = to_string(new_quantity);
                        rowValues[6] = timestamp;
                    } else if (rowValues.getSize() == 6) {
                        rowValues[3] = to_string(new_quantity);
                        rowValues.PUSH(timestamp);
                    }
                    Vec updatedValue;
                    string updatedRow;
                    for (size_t j = 0; j < rowValues.getSize(); j++) {
                        if (j > 0) updatedRow += ",";
                        updatedRow += rowValues[j];
                    }
                    updatedValue.PUSH(updatedRow);
                    orderHash[i].setValue(updatedValue);
                    string tablePath = currentSchemaName + "/order";
                    for (const auto& entry : filesystem::directory_iterator(tablePath)) {
                        string filename = entry.path().filename().string();
                        if (entry.is_regular_file() && 
                            filename.length() >= 4 && 
                            filename.substr(filename.length() - 4) == ".csv") {
                            string csvFilePath = entry.path().string();
                            string tempFilePath = csvFilePath + ".tmp";
                            ifstream inputFile(csvFilePath);
                            ofstream outputFile(tempFilePath);
                            if (!inputFile.is_open() || !outputFile.is_open()) continue;
                            string line;
                            bool firstLine = true;
                            while (getline(inputFile, line)) {
                                if (firstLine) {
                                    outputFile << line << endl;
                                    firstLine = false;
                                    continue;
                                }
                                if (line.find(to_string(order_id) + ",") == 0) {
                                    outputFile << updatedRow << endl;
                                } else {
                                    outputFile << line << endl;
                                }
                            }
                            inputFile.close();
                            outputFile.close();
                            filesystem::remove(csvFilePath);
                            filesystem::rename(tempFilePath, csvFilePath);
                            return true;
                        }
                    }
                }
            }
        }
    }
    cout << "Ордер не найден" << endl;
    return false;
}
