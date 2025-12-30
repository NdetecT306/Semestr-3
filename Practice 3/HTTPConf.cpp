#include "HTTPConf.h"
using json = nlohmann::json;
extern string currentSchemaName;
extern json schemaConfig;
namespace fs = std::filesystem;
atomic<bool> http_server_running{true};
// Функция для получения всех пар в формате JSON
string get_pairs_json() {
    json result = json::array();
    DOUBLE_HASH* pairHash = GET_SCHEMA("pair");
    if (pairHash == nullptr) {
        return "[]";
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (pairHash[i].isValid()) {
            Vec rowValues;
            string rowData = pairHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 3) {
                json pair_obj;
                pair_obj["pair_id"] = stoi(rowValues[0]);
                pair_obj["sale_lot_id"] = stoi(rowValues[1]);
                pair_obj["buy_lot_id"] = stoi(rowValues[2]);
                result.push_back(pair_obj);
            }
        }
    }
    return result.dump(4);
}

// Функция для получения всех лотов в формате JSON
string get_lots_json() {
    json result = json::array();
    DOUBLE_HASH* lotHash = GET_SCHEMA("lot");
    if (lotHash == nullptr) {
        return "[]";
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (lotHash[i].isValid()) {
            Vec rowValues;
            string rowData = lotHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 2) {
                json lot_obj;
                lot_obj["lot_id"] = stoi(rowValues[0]);
                lot_obj["name"] = rowValues[1];
                result.push_back(lot_obj);
            }
        }
    }
    return result.dump(4);
}

string get_orders_handler() {
    json response = json::array();
    // Сначала загружаем актуальные данные
    CLEAR_SCHEMA_TABLE();
    loadAllDataToHash();
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        cout << "Таблица 'order' не найдена в SCHEMA_HASH" << endl;
        return "[]";
    }
    int foundOrders = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (orderHash[i].isValid()) {
            foundOrders++;
            Vec rowValues;
            string rowData = orderHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            // Проверяем минимум 6 полей (closed)
            if (rowValues.getSize() >= 6) {
                try {
                    json order;
                    order["order_id"] = stoi(rowValues[0]);
                    order["user_id"] = stoi(rowValues[1]);
                    order["quantity"] = stof(rowValues[3]);
                    order["price"] = stof(rowValues[4]);
                    order["type"] = rowValues[5];
                    if (rowValues.getSize() >= 7) {
                        string closedValue = rowValues[6];
                        if (closedValue.empty()) {
                            order["closed"] = "";
                        } else {
                            order["closed"] = closedValue;
                        }
                    } else {
                        order["closed"] = ""; // Пустая строка если нет поля
                    }
                    // Получаем информацию о паре для лотов
                    int pair_id = stoi(rowValues[2]);
                    PairInfo pairInfo = getPairInfo(pair_id);
                    if (pairInfo.pair_id != -1) {
                        order["lot_id"] = pairInfo.first_lot_id;
                    }
                    response.push_back(order);  
                } catch (const exception& e) {
                    cout << "Ошибка парсинга ордера: " << e.what() << ", данные: " << rowData << endl;
                }
            } else {
                cout << "Неверный формат строки ордера, нужно минимум 6 полей, а найдено " << rowValues.getSize() << " " << rowData << endl;
            }
        }
    }
    return response.dump(4);
}
string create_user_handler(const json& request_data) {
    json response;
    if (!request_data.contains("username") || !request_data["username"].is_string() ||request_data["username"].get<string>().empty()) {
        response["error"] = "Не указано имя пользователя или оно пустое";
        return response.dump(4);
    }
    string username = request_data["username"];
    // Проверяем, не существует ли уже пользователь с таким именем
    string existingUserId = getUserIdByUsername(username);
    if (existingUserId != "") {
        response["error"] = "Пользователь с таким именем уже существует";
        response["existing_user_id"] = existingUserId;
        return response.dump(4);
    }
    // Генерируем ключ
    string userKey = generateUserKey();
    Vec userValues;
    userValues.PUSH(username);  
    userValues.PUSH(userKey);    
    string userId = insertIntoCSV("user", userValues);
    if (userId == "-1" || userId.empty()) {
        response["error"] = "Ошибка при создании пользователя в БД";
        return response.dump(4);
    }
    CLEAR_SCHEMA_TABLE();
    loadAllDataToHash();
    this_thread::sleep_for(chrono::milliseconds(200));
    // Проверяем, что пользователь действительно добавлен
    string checkUserId = getUserIdByKey(userKey);
    if (checkUserId == "") {
        cout << "ОШИБКА: Пользователь не найден после создания!" << endl;
        checkUserId = getUserIdByUsername(username);
        if (checkUserId != "") {
            cout << "Найден по username, ID: " << checkUserId << endl;
        }
        response["error"] = "Пользователь создан, но не найден в системе";
        response["user_id"] = userId;
        response["key"] = userKey;
        return response.dump(4);
    }
    // Инициализируем баланс пользователя 
    if (!initializeUserBalance(checkUserId)) {
        cerr << "Не удалось полностью инициализировать баланс пользователя " << checkUserId << endl;
    }
    CLEAR_SCHEMA_TABLE();
    loadAllDataToHash();
    this_thread::sleep_for(chrono::milliseconds(100));
    // Проверяем баланс
    DOUBLE_HASH* userLotHash = GET_SCHEMA("user_lot");
    int balanceCount = 0;
    if (userLotHash != nullptr) {
        for (int i = 0; i < HASH_SIZE; i++) {
            if (userLotHash[i].isValid()) {
                Vec rowValues;
                string rowData = userLotHash[i].getValue()[0];
                stringstream ss(rowData);
                string cell;
                while (getline(ss, cell, ',')) {
                    rowValues.PUSH(cell);
                }
                if (rowValues.getSize() >= 3 && rowValues[0] == checkUserId) {
                    balanceCount++;
                }
            }
        }
    }
    response["key"] = userKey;
    return response.dump(4);
}
string create_order_handler(const string& userKey, const json& request_data) {
    json response;
    // ПРОВЕРКА ПОЛЬЗОВАТЕЛЯ
    string userId = getUserIdByKey(userKey);
    if (userId == "") {
        response["error"] = "Неверный ключ пользователя";
        return response.dump(4);
    }
    int user_id = stoi(userId);
    // ПРОВЕРКА ПОЛЕЙ ЗАПРОСА
    if (!request_data.contains("pair_id") || 
        !request_data.contains("quantity") || 
        !request_data.contains("price") || 
        !request_data.contains("type") || 
        !request_data["type"].is_string()) {
        response["error"] = "Отсутствуют обязательные поля: pair_id, quantity, price, type";
        return response.dump(4);
    }
    int pair_id;
    float quantity, price;
    string type;
    try {
        pair_id = request_data["pair_id"].get<int>();
        quantity = request_data["quantity"].get<float>();
        price = request_data["price"].get<float>();
        type = request_data["type"].get<string>();
    } catch (const exception& e) {
        response["error"] = string("Неверный формат данных: ") + e.what();
        return response.dump(4);
    }
    // ВАЛИДАЦИЯ
    if (!validateOrderParameters(pair_id, quantity, price, type)) {
        response["error"] = "Неверные параметры ордера";
        return response.dump(4);
    }
    if (!checkUserBalanceForOrder(userKey, pair_id, quantity, price, type)) {
        response["error"] = "Недостаточно средств";
        return response.dump(4);
    }
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        response["error"] = "Указанная пара не существует";
        return response.dump(4);
    }
    // РЕЗЕРВИРОВАНИЕ СРЕДСТВ
    if (!reserveFundsForOrder(user_id, pair_id, quantity, price, type)) {
        response["error"] = "Не удалось зарезервировать средства";
        return response.dump(4);
    }
    CLEAR_SCHEMA_TABLE();
    loadAllDataToHash();
    Order newOrder;
    newOrder.user_id = user_id;
    newOrder.pair_id = pair_id;
    newOrder.quantity = quantity;
    newOrder.price = price;
    newOrder.type = type;
    newOrder.closed = "";
    // Ищем встречный ордер с выгодной ценой для покупателя
    Order* matchingOrder = findMatchingOrder(newOrder);
    vector<int> created_order_ids;
    time_t now = time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d", localtime(&now));
    if (matchingOrder != nullptr) {
        // Определяем покупателя и продавца
        int buyer_id, seller_id;
        float execution_price = matchingOrder->execution_price;
        // Определяем кто покупатель, а кто продавец
        if (type == "buy") {
            buyer_id = user_id;
            seller_id = matchingOrder->user_id;
        } else {
            buyer_id = matchingOrder->user_id;
            seller_id = user_id;
        }
        // Сравниваем количества
        float execute_quantity = min(quantity, matchingOrder->quantity);
        // Определяем оригинальную цену для расчета возврата
        float original_price_for_refund = 0.0f;
        int refund_user_id = -1;
        if (type == "buy") {
            original_price_for_refund = price;
            refund_user_id = user_id;
        } else {
            original_price_for_refund = matchingOrder->price;
            refund_user_id = matchingOrder->user_id;
        }
        // Исполняем сделку (без возврата средств здесь)
        if (processOrderExecution(buyer_id, seller_id, pair_id, execute_quantity, execution_price,original_price_for_refund)) {
            // Если количества равны
            if (fabs(quantity - matchingOrder->quantity) < 0.001) {
                Vec newOrderValues;
                newOrderValues.PUSH(to_string(user_id));
                newOrderValues.PUSH(to_string(pair_id));
                newOrderValues.PUSH(to_string(quantity));
                newOrderValues.PUSH(to_string(execution_price));
                newOrderValues.PUSH(type);
                newOrderValues.PUSH(timeStr);
                string newOrderId = insertIntoCSV("order", newOrderValues);
                if (newOrderId != "-1" && !newOrderId.empty()) {
                    created_order_ids.push_back(stoi(newOrderId));
                }
                // Встречный ордер закрываем
                closeOrder(matchingOrder->order_id, timeStr);
                // ВОЗВРАТ СРЕДСТВ если цена исполнения ниже оригинальной
                if (original_price_for_refund > execution_price + 0.001) {
                    float expected_refund = (original_price_for_refund - execution_price) * execute_quantity;
                    if (!refundExcessFundsToBuyer(refund_user_id, pair_id, original_price_for_refund, execution_price,  execute_quantity)) {
                        cout << "Не удалось вернуть излишние средства" << endl;
                    }
                }
            } 
            // Если новый ордер больше
            else if (quantity > matchingOrder->quantity) {
                // Встречный ордер закрываем (полностью исполнен)
                closeOrder(matchingOrder->order_id, timeStr);
                Vec executedPartValues;
                executedPartValues.PUSH(to_string(user_id));
                executedPartValues.PUSH(to_string(pair_id));
                executedPartValues.PUSH(to_string(execute_quantity));
                executedPartValues.PUSH(to_string(execution_price)); 
                executedPartValues.PUSH(type);
                executedPartValues.PUSH(timeStr);
                string executedPartId = insertIntoCSV("order", executedPartValues);
                if (executedPartId != "-1" && !executedPartId.empty()) {
                    created_order_ids.push_back(stoi(executedPartId));
                }
                float remaining_quantity = quantity - execute_quantity;
                Vec remainingPartValues;
                remainingPartValues.PUSH(to_string(user_id));
                remainingPartValues.PUSH(to_string(pair_id));
                remainingPartValues.PUSH(to_string(remaining_quantity));
                remainingPartValues.PUSH(to_string(price)); 
                remainingPartValues.PUSH(type);
                remainingPartValues.PUSH("");
                string remainingPartId = insertIntoCSV("order", remainingPartValues);
                if (remainingPartId != "-1" && !remainingPartId.empty()) {
                    created_order_ids.push_back(stoi(remainingPartId));
                }
                // ВОЗВРАТ СРЕДСТВ для исполненной части
                if (original_price_for_refund > execution_price + 0.001) {
                    float expected_refund = (original_price_for_refund - execution_price) * execute_quantity;
                    if (!refundExcessFundsToBuyer(refund_user_id, pair_id, original_price_for_refund, execution_price, execute_quantity)) {
                        cout << "Не удалось вернуть излишние средства" << endl;
                    }
                }
            } 
            // Если встречный ордер больше
            else {
                Vec newOrderValues;
                newOrderValues.PUSH(to_string(user_id));
                newOrderValues.PUSH(to_string(pair_id));
                newOrderValues.PUSH(to_string(quantity));
                newOrderValues.PUSH(to_string(execution_price)); 
                newOrderValues.PUSH(type);
                newOrderValues.PUSH(timeStr);
                string newOrderId = insertIntoCSV("order", newOrderValues);
                if (newOrderId != "-1" && !newOrderId.empty()) {
                    created_order_ids.push_back(stoi(newOrderId));
                }
                updateOrderQuantityAndClose(matchingOrder->order_id, execute_quantity, timeStr);
                float remaining_quantity = matchingOrder->quantity - execute_quantity;
                Vec remainingCounterPartValues;
                remainingCounterPartValues.PUSH(to_string(matchingOrder->user_id));
                remainingCounterPartValues.PUSH(to_string(pair_id));
                remainingCounterPartValues.PUSH(to_string(remaining_quantity));
                remainingCounterPartValues.PUSH(to_string(matchingOrder->price)); 
                remainingCounterPartValues.PUSH(matchingOrder->type);
                remainingCounterPartValues.PUSH("");
                string remainingCounterPartId = insertIntoCSV("order", remainingCounterPartValues);
                if (remainingCounterPartId != "-1" && !remainingCounterPartId.empty()) {
                    created_order_ids.push_back(stoi(remainingCounterPartId));
                }
                // ВОЗВРАТ СРЕДСТВ (весь новый ордер исполнен)
                if (original_price_for_refund > execution_price + 0.001) {
                    float expected_refund = (original_price_for_refund - execution_price) * execute_quantity;
                    if (!refundExcessFundsToBuyer(refund_user_id, pair_id, original_price_for_refund, execution_price, execute_quantity)) {
                        cout << "Не удалось вернуть излишние средства" << endl;
                    }
                }
            }
        } else {
            cout << "Ошибка исполнения сделки" << endl;
            returnReservedFunds(user_id, pair_id, quantity, price, type);
            response["error"] = "Ошибка исполнения сделки";
            response["success"] = false;
            delete matchingOrder;
            return response.dump(4);
        }
        delete matchingOrder;
    } else {
        // Создаем открытый ордер
        Vec orderValues;
        orderValues.PUSH(to_string(user_id));
        orderValues.PUSH(to_string(pair_id));
        orderValues.PUSH(to_string(quantity));
        orderValues.PUSH(to_string(price));
        orderValues.PUSH(type);
        orderValues.PUSH("");
        string orderId = insertIntoCSV("order", orderValues);
        if (orderId != "-1" && !orderId.empty()) {
            created_order_ids.push_back(stoi(orderId));
        }
    }
    CLEAR_SCHEMA_TABLE();
    loadAllDataToHash();
    // ФОРМИРОВАНИЕ ОТВЕТА
    if (!created_order_ids.empty() || matchingOrder == nullptr) {
        response["order_ids"] = created_order_ids;
    } else {
        response["error"] = "Не удалось создать ордер в БД";
    }
    return response.dump(4);
}
struct OrderInfo {
        int user_id;
        int pair_id;
        float quantity;
        float price;
        string type;
};
// Функция для удаления ордера
string delete_order_handler(const string& userKey, const json& request_data) {
    json response;
    // ПРОВЕРКА ПОЛЬЗОВАТЕЛЯ
    string userId = getUserIdByKey(userKey);
    if (userId == "") {
        response["error"] = "Неверный ключ пользователя";
        return response.dump(4);
    }
    // ПРОВЕРКА ОБЯЗАТЕЛЬНЫХ ПОЛЕЙ
    if (!request_data.contains("order_id")) {
        response["error"] = "Отсутствует обязательное поле: order_id";
        return response.dump(4);
    }
    int order_id;
    try {
        if (request_data["order_id"].is_number_integer()) {
            order_id = request_data["order_id"].get<int>();
        } else if (request_data["order_id"].is_string()) {
            order_id = stoi(request_data["order_id"].get<string>());
        } else {
            response["error"] = "order_id должен быть строкой или числом";
            return response.dump(4);
        }
    } catch (const exception& e) {
        response["error"] = string("Неверный формат order_id: ") + e.what();
        return response.dump(4);
    }
    DOUBLE_HASH* orderHash = GET_SCHEMA("order");
    if (orderHash == nullptr) {
        response["error"] = "Таблица ордеров недоступна";
        return response.dump(4);
    }
    bool orderFound = false;
    OrderInfo orderInfo;
    int user_id_int = stoi(userId);
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
                int currentOrderId;
                try {
                    currentOrderId = stoi(rowValues[0]);
                } catch (...) {
                    continue;
                }
                if (currentOrderId == order_id) {
                    int currentUserId;
                    try {
                        currentUserId = stoi(rowValues[1]);
                    } catch (...) {
                        response["error"] = "Неверный формат user_id в ордере";
                        return response.dump(4);
                    }
                    if (currentUserId != user_id_int) {
                        response["error"] = "Ордер принадлежит другому пользователю";
                        return response.dump(4);
                    }
                    // Проверяем, не закрыт ли уже ордер
                    if (rowValues.getSize() >= 7 && !rowValues[6].empty()) {
                        response["error"] = "Ордер уже закрыт, отмена невозможна";
                        response["closed_at"] = rowValues[6];
                        return response.dump(4);
                    }
                    orderFound = true;
                    // Заполняем информацию об ордере
                    try {
                        orderInfo.user_id = currentUserId;
                        orderInfo.pair_id = stoi(rowValues[2]);
                        orderInfo.quantity = stof(rowValues[3]);
                        orderInfo.price = stof(rowValues[4]);
                        orderInfo.type = rowValues[5];
                    } catch (const exception& e) {
                        response["error"] = string("Ошибка парсинга данных ордера: ") + e.what();
                        return response.dump(4);
                    }
                    break;
                }
            }
        }
    }
    if (!orderFound) {
        response["error"] = "Ордер не найден или уже удален";
        return response.dump(4);
    }
    if (!returnReservedFunds(orderInfo.user_id, orderInfo.pair_id, orderInfo.quantity, orderInfo.price, orderInfo.type)) {
        response["error"] = "Не удалось вернуть зарезервированные средства";
        return response.dump(4);
    }
    // ПОМЕТКА ОРДЕРА КАК ЗАКРЫТОГО С ПРИЧИНОЙ ОТМЕНЫ
    bool updated = false;
    time_t now = time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d", localtime(&now));
    // Формируем отметку о закрытии с причиной
    string closed_mark = string(timeStr) + " (отменен пользователем)";
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
                int currentOrderId;
                try {
                    currentOrderId = stoi(rowValues[0]);
                } catch (...) {
                    continue;
                }
                if (currentOrderId == order_id) {
                    // Обновляем поле closed
                    if (rowValues.getSize() >= 7) {
                        rowValues[6] = closed_mark;
                    } else if (rowValues.getSize() == 6) {
                        rowValues.PUSH(closed_mark);
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
                    for (const auto& entry : fs::directory_iterator(tablePath)) {
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
                                    // Нашли строку с нашим ордером
                                    Vec columns;
                                    stringstream line_ss(line);
                                    string column;
                                    while (getline(line_ss, column, ',')) {
                                        columns.PUSH(column);
                                    }
                                    if (columns.getSize() >= 7) {
                                        columns[6] = closed_mark;
                                    } else if (columns.getSize() == 6) {
                                        columns.PUSH(closed_mark);
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
                            fs::remove(csvFilePath);
                            fs::rename(tempFilePath, csvFilePath);
                            updated = true;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    if (updated) {
        CLEAR_SCHEMA_TABLE();
        loadAllDataToHash();
    } else {
        response["error"] = "Не удалось обновить ордер в базе данных";
        response["success"] = false;
    }
    return response.dump(4);
}
string get_balance_handler(const string& userKey) {
    json response = json::array();
    // Проверяем пользователя
    string userIdStr = getUserIdByKey(userKey);
    if (userIdStr == "") {
        json error;
        error["error"] = "Неверный ключ пользователя";
        cout << "Пользователь не найден" << endl;
        return error.dump(4);
    }
    int userId = stoi(userIdStr);
    DOUBLE_HASH* userLotHash = GET_SCHEMA("user_lot");
    if (userLotHash == nullptr) {
        cout << "Таблица user_lot не найдена" << endl;
        return response.dump(4);
    }
    int foundRecords = 0;
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
                int currentUserId = stoi(rowValues[0]);
                if (currentUserId == userId) {
                    json balanceItem;
                    balanceItem["lot_id"] = stoi(rowValues[1]);
                    balanceItem["quantity"] = stof(rowValues[2]);
                    response.push_back(balanceItem);
                    foundRecords++;
                }
            } else {
                cout << "Пропуск строки: " << rowData << endl;
            }
        }
    }
    return response.dump(4);
}
// Разбор HTTP-запроса
string parse_http_request(const string& request) {
    if (request.empty()) {
        json status;
        status["status"] = "OK";
        status["schema"] = currentSchemaName;
        status["timestamp"] = time(nullptr);
        status["message"] = "Empty request";
        return status.dump(4);
    }
    stringstream ss(request);
    string line;
    string method;
    string path;
    // Читаем первую строку запроса
    if (getline(ss, line)) {
        stringstream first_line(line);
        first_line >> method >> path;
        // Удаляем возможные пробелы в конце
        method.erase(method.find_last_not_of(" \t\n\r") + 1);
        path.erase(path.find_last_not_of(" \t\n\r") + 1);
    } else {
        // Если не удалось прочитать строку, возвращаем статус
        json status;
        status["status"] = "OK";
        status["schema"] = currentSchemaName;
        status["timestamp"] = time(nullptr);
        return status.dump(4);
    }
    // Чтение заголовков
    map<string, string> headers;
    while (getline(ss, line) && !line.empty() && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string header_name = line.substr(0, colon_pos);
            string header_value = line.substr(colon_pos + 1);
            header_name.erase(0, header_name.find_first_not_of(" \t\r\n"));
            header_name.erase(header_name.find_last_not_of(" \t\r\n") + 1);
            header_value.erase(0, header_value.find_first_not_of(" \t\r\n"));
            header_value.erase(header_value.find_last_not_of(" \t\r\n") + 1);
            headers[header_name] = header_value;
        }
    }
    // Чтение тела запроса 
    string body;
    if (headers.find("Content-Length") != headers.end()) {
        try {
            int content_length = stoi(headers["Content-Length"]);
            if (content_length > 0) {
                vector<char> buffer(content_length);
                ss.read(buffer.data(), content_length);
                body = string(buffer.data(), content_length);
            }
        } catch (...) {}
    }
    cout << "HTTP запрос: " << method << " " << path << endl;
    if (!body.empty()) {
        cout << "Тело запроса: " << body << endl;
    }
    // Обработка различных методов и путей
    if (method == "GET") {
        if (path == "/pair" || path == "/pair/") {
            return get_pairs_json();
        } else if (path == "/lot" || path == "/lot/") {
            return get_lots_json();
        } else if (path == "/order" || path == "/order/") {
            return get_orders_handler();
        } else if (path == "/balance" || path == "/balance/") {
            // Проверяем заголовок X-USER-KEY
            if (headers.find("X-USER-KEY") == headers.end()) {
                json error;
                error["error"] = "Требуется заголовок X-USER-KEY";
                return error.dump(4);
            }
            return get_balance_handler(headers["X-USER-KEY"]);
        } else if (path == "/" || path == "/status") {
            json status;
            status["status"] = "OK";
            status["schema"] = currentSchemaName;
            status["timestamp"] = time(nullptr);
            return status.dump(4);
        } else {
            json error;
            error["error"] = "Not Found";
            error["message"] = "Ресурс " + path + " не найден";
            error["timestamp"] = time(nullptr);
            return error.dump(4);
        }
    } else if (method == "POST") {
        json request_json;
        try {
            if (!body.empty()) {
                request_json = json::parse(body);
            }
        } catch (const exception& e) {
            json error;
            error["error"] = "Неверный формат JSON";
            error["message"] = e.what();
            return error.dump(4);
        }
        if (path == "/user" || path == "/user/") {
            return create_user_handler(request_json);
        } else if (path == "/order" || path == "/order/") {
            // Проверяем заголовок X-USER-KEY
            if (headers.find("X-USER-KEY") == headers.end()) {
                json error;
                error["error"] = "Требуется заголовок X-USER-KEY";
                return error.dump(4);
            }
            return create_order_handler(headers["X-USER-KEY"], request_json);
        } else {
            json error;
            error["error"] = "Not Found";
            error["message"] = "Ресурс " + path + " не найден";
            return error.dump(4);
        }
    } else if (method == "DELETE") {
        json request_json;
        try {
            if (!body.empty()) {
                request_json = json::parse(body);
            }
        } catch (const exception& e) {
            json error;
            error["error"] = "Неверный формат JSON";
            error["message"] = e.what();
            return error.dump(4);
        }
        if (path == "/order" || path == "/order/") {
            // Проверяем заголовок X-USER-KEY
            if (headers.find("X-USER-KEY") == headers.end()) {
                json error;
                error["error"] = "Требуется заголовок X-USER-KEY";
                return error.dump(4);
            }
            return delete_order_handler(headers["X-USER-KEY"], request_json);
        } else {
            json error;
            error["error"] = "Not Found";
            error["message"] = "Ресурс " + path + " не найден";
            return error.dump(4);
        }
    } else {
        json error;
        error["error"] = "Method Not Allowed";
        error["message"] = "Метод " + method + " не поддерживается";
        error["allowed_methods"] = {"GET", "POST", "DELETE"};
        error["timestamp"] = time(nullptr);
        return error.dump(4);
    }
}
// Формирование HTTP-ответа
string create_http_response(const string& content) {
    stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Server: Exchange-HTTP-Server/1.0\r\n";
    response << "Content-Type: application/json; charset=utf-8\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "\r\n";  
    response << content;
    return response.str();
}
// Формирование HTTP-ответа с ошибкой
string create_http_error_response(int status_code, const string& message) {
    json error;
    error["error"] = true;
    error["status_code"] = status_code;
    error["message"] = message;
    error["timestamp"] = time(nullptr);
    string content = error.dump(4);
    stringstream response;
    response << "HTTP/1.1 " << status_code << " ";
    switch(status_code) {
        case 400: response << "Bad Request"; break;
        case 404: response << "Not Found"; break;
        case 405: response << "Method Not Allowed"; break;
        case 500: response << "Internal Server Error"; break;
        default: response << "Error"; break;
    }
    response << "\r\n";
    response << "Server: Exchange-HTTP-Server/1.0\r\n";
    response << "Content-Type: application/json; charset=utf-8\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "\r\n";
    response << content;
    return response.str();
}
// Обработка HTTP-клиента
void handle_http_client(int client_socket) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    // Читаем HTTP-запрос
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    string request(buffer, bytes_received);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    // Обрабатываем запрос и формируем JSON-ответ
    string json_response;
    string http_response;
    try {
        json_response = parse_http_request(request);
        http_response = create_http_response(json_response);
    } catch (const exception& e) {
        cerr << "Ошибка обработки запроса: " << e.what() << endl;
        http_response = create_http_error_response(500, "Internal Server Error: " + string(e.what()));
    }
    // Отправляем ответ
    ssize_t bytes_sent = send(client_socket, http_response.c_str(), http_response.length(), 0);
    if (bytes_sent < 0) {
        cerr << "Ошибка отправки ответа клиенту " << client_ip << ": " << strerror(errno) << endl;
    }
    // Закрываем соединение
    close(client_socket);
}
// Запуск HTTP-сервера
void start_http_server(int port) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Ошибка создания HTTP сокета: " << strerror(errno) << endl;
        return;
    }
    // Настраиваем опцию повторного использования адреса
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Ошибка установки SO_REUSEADDR: " << strerror(errno) << endl;
    }
    // Настраиваем адрес
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    // Привязываем сокет
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка привязки HTTP сокета к порту " << port << ": " << strerror(errno) << endl;
        close(server_socket);
        return;
    }
    if (listen(server_socket, 10) < 0) {
        cerr << "Ошибка начала прослушивания HTTP порта: " << strerror(errno) << endl;
        close(server_socket);
        return;
    }
    cout << "Адрес: http://localhost:" << port << endl;
    cout << "Порт: " << port << endl;
    cout << "Доступные ресурсы:" << endl;
    cout << "  GET /pair   - получение списка пар" << endl;
    cout << "  GET /lot    - получение списка лотов" << endl;
    cout << "  GET /order  - получение списка ордеров" << endl;
    cout << "  GET /balance - баланс пользователя (X-USER-KEY)" << endl;
    cout << "  POST /user  - создание пользователя" << endl;
    cout << "  POST /order - создание ордера (X-USER-KEY)" << endl;
    cout << "  DELETE /order - удаление ордера (X-USER-KEY)" << endl;
    while (http_server_running) {
        // Принимаем соединение
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (http_server_running) {
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }
            break;
        }
        // Выводим информацию о подключении
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        cout << "HTTP подключение: " << client_ip << ":" << ntohs(client_addr.sin_port) << endl;
        thread client_thread(handle_http_client, client_socket);
        client_thread.detach();
    }
    close(server_socket);
    cout << "HTTP сервер остановлен" << endl;
}
// Остановка HTTP-сервера
void stop_http_server() {
    http_server_running = false;
    this_thread::sleep_for(chrono::milliseconds(500));
    int temp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (temp_socket >= 0) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(8080);
        connect(temp_socket, (struct sockaddr*)&addr, sizeof(addr));
        this_thread::sleep_for(chrono::milliseconds(100));
        close(temp_socket);
    }
    cout << "Сигнал остановки HTTP сервера отправлен" << endl;
}
