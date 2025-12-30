#include "balanceOperations.h"
extern string currentSchemaName;

// Функция для получения информации о паре
PairInfo getPairInfo(int pair_id) {
    PairInfo info;
    info.pair_id = -1;
    info.first_lot_id = -1;
    info.second_lot_id = -1;
    info.first_lot_name = "";
    info.second_lot_name = "";
    DOUBLE_HASH* pairHash = GET_SCHEMA("pair");
    if (pairHash == nullptr) {
        return info;
    }
    // Ищем пару
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
                int current_pair_id = stoi(rowValues[0]);
                if (current_pair_id == pair_id) {
                    info.pair_id = current_pair_id;
                    info.first_lot_id = stoi(rowValues[1]);
                    info.second_lot_id = stoi(rowValues[2]);
                    break;
                }
            }
        }
    }
    if (info.pair_id == -1) {
        return info;
    }
    // Получаем названия лотов
    DOUBLE_HASH* lotHash = GET_SCHEMA("lot");
    if (lotHash != nullptr) {
        for (int j = 0; j < HASH_SIZE; j++) {
            if (lotHash[j].isValid()) {
                Vec lotValues;
                string lotData = lotHash[j].getValue()[0];
                stringstream lot_ss(lotData);
                string lot_cell;
                while (getline(lot_ss, lot_cell, ',')) {
                    lotValues.PUSH(lot_cell);
                }
                if (lotValues.getSize() >= 2) {
                    int lot_id = stoi(lotValues[0]);
                    string lot_name = lotValues[1];
                    if (lot_id == info.first_lot_id) {
                        info.first_lot_name = lot_name;
                    } else if (lot_id == info.second_lot_id) {
                        info.second_lot_name = lot_name;
                    }
                }
            }
        }
    }
    // Если названия не нашли, используем ID как строку
    if (info.first_lot_name.empty()) {
        info.first_lot_name = to_string(info.first_lot_id);
    }
    if (info.second_lot_name.empty()) {
        info.second_lot_name = to_string(info.second_lot_id);
    }
    return info;
}
// Функция для возврата излишне зарезервированных средств покупателю
bool refundExcessFundsToBuyer(int buyer_id, int pair_id,float original_price, float execution_price,float executed_quantity) {
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        cout << "Пара не найдена." << endl;
        return false;
    }
    if (original_price <= execution_price + 0.001) {
        cout << "Цена исполнения не ниже оригинальной, возврат не требуется, все покрылось." << endl;
        return true;
    }
    float price_difference = original_price - execution_price;
    float refund_amount = price_difference * executed_quantity;
    if (!updateUserBalance(buyer_id, pairInfo.second_lot_id, refund_amount)) {
        cout << "Ошибка возврата средств покупателю." << endl;
        return false;
    }
    return true;
}
// Обработка исполнения ордера с возвратом излишних средств
bool processOrderExecution(int buyer_id, int seller_id, int pair_id,float quantity, float execution_price,float original_price) {
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        cout << "Пара не найдена." << endl;
        return false;
    }
    if (original_price > 0.001) {
        float difference = original_price - execution_price;
        if (difference > 0.001) {
            float potential_refund = difference * quantity;
        }
    }
    // Рассчитываем суммы
    float first_lot_amount = quantity;           
    float second_lot_amount = quantity * execution_price;  // Оплата по цене исполнения
    // Покупатель получает first_lot 
    if (!updateUserBalance(buyer_id, pairInfo.first_lot_id, first_lot_amount)) {
        cout << "Не удалось обновить баланс покупателя." << endl;
        return false;
    }
    // Продавец получает second_lot 
    if (!updateUserBalance(seller_id, pairInfo.second_lot_id, second_lot_amount)) {
        cout << "Не удалось обновить баланс продавца." << endl;
        // Откат первой операции
        updateUserBalance(buyer_id, pairInfo.first_lot_id, -first_lot_amount);
        return false;
    }    
    return true;
}
// Перегруженная версия для обратной совместимости
bool processOrderExecution(int buyer_id, int seller_id, int pair_id,float quantity, float execution_price) {
    return processOrderExecution(buyer_id, seller_id, pair_id,quantity, execution_price, 0.0f);
}
// Проверка баланса пользователя для создания ордера
bool checkUserBalanceForOrder(const string& userKey, int pair_id, float quantity, float price, const string& type) {
    string userId = getUserIdByKey(userKey);
    if (userId.empty()) {
        return false;
    }
    int user_id = stoi(userId);
    // Получаем информацию о паре
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        return false;
    }
    // Проверяем баланс в зависимости от типа ордера
    if (type == "buy") {
        float required_amount = quantity * price;
        float available_balance = getUserBalance(user_id, pairInfo.second_lot_id);
        if (available_balance < required_amount) {
            cout << "Недостаточно средств для покупателя. Смотри за своими финансами самостоятельно." << endl;
            return false;
        }
    } else if (type == "sell") {
        float required_amount = quantity;
        float available_balance = getUserBalance(user_id, pairInfo.first_lot_id);
        if (available_balance < required_amount) {
            cout << "Недостаточно средств для продавца. Не повезло." << endl;
            return false;
        }
    } else {
        return false;
    }
    return true;
}
// Функция для резервирования средств при создании ордера
bool reserveFundsForOrder(int user_id, int pair_id, float quantity, float price, const string& type) {
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        cerr << "Пара не найдена, pair_id = " << pair_id << endl;
        return false;
    }
    if (type == "buy") {
        float required_amount = quantity * price;
        // Списание средств при резервировании
        if (!updateUserBalance(user_id, pairInfo.second_lot_id, -required_amount)) {
            cerr << "Не удалось зарезервировать средства для покупки" << endl;
            return false;
        }
    } else if (type == "sell") {
        float required_amount = quantity;
        // Списание средств при резервировании 
        if (!updateUserBalance(user_id, pairInfo.first_lot_id, -required_amount)) {
            cerr << "Не удалось зарезервировать средства для продажи" << endl;
            return false;
        }
    } else {
        cerr << "Неверный тип ордера: " << type << endl;
        return false;
    }
    return true;
}
// Функция для возврата средств при отмене ордера
bool returnReservedFunds(int user_id, int pair_id, float quantity, float price, const string& type) {
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        cerr << "Пара не найдена, pair_id = " << pair_id << endl;
        return false;
    }
    
    if (type == "buy") {
        // Для BUY ордера возвращаем зарезервированную сумму (количество × цена)
        float amount_to_return = quantity * price;
        if (!updateUserBalance(user_id, pairInfo.second_lot_id, amount_to_return)) {
            cerr << "Ошибка возврата средств для BUY ордера" << endl;
            return false;
        }
    } else if (type == "sell") {
        // Для SELL ордера возвращаем зарезервированное количество
        if (!updateUserBalance(user_id, pairInfo.first_lot_id, quantity)) {
            cerr << "Ошибка возврата средств для SELL ордера" << endl;
            return false;
        }
    } else {
        cerr << "Неверный тип ордера: " << type << endl;
        return false;
    }
    return true;
}
// Валидация параметров ордера
bool validateOrderParameters(int pair_id, float quantity, float price, const string& type) {
    if (quantity <= 0) {
        cout << "Количество денег должно быть больше 0. Мы не микрозаймы раздаем" << endl;
        return false;
    }
    if (price <= 0) {
        cout << "Цена должна быть больше 0. Бесплатный только сыр в мышеловке" << endl;
        return false;
    }
    if (type != "buy" && type != "sell") {
        cout << "Тип должен быть 'buy' или 'sell'. Классический рынок" << endl;
        return false;
    }
    // Проверяем существование пары
    PairInfo pairInfo = getPairInfo(pair_id);
    if (pairInfo.pair_id == -1) {
        cout << "Валютная пара не найдена" << endl;
        return false;
    }
    return true;
}

