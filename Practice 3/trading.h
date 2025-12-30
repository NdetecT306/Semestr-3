#ifndef TRADING_LOGIC_H
#define TRADING_LOGIC_H

#include <vector>
#include <string>
#include <utility>
#include "balanceOperations.h"
#include "MYCSV.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include "json.hpp"
using namespace std;


struct Order {
    int order_id;
    int user_id;
    int pair_id;
    float quantity;
    float price;
    string type;
    string closed;
    float execution_price; 
    Order() : order_id(-1), user_id(-1), pair_id(-1), quantity(0.0f), price(0.0f), type(""), closed(""), execution_price(0.0f) {}
    Order(const Order& other) {
        order_id = other.order_id;
        user_id = other.user_id;
        pair_id = other.pair_id;
        quantity = other.quantity;
        price = other.price;
        type = other.type;
        closed = other.closed;
        execution_price = other.execution_price;
    }
};
struct PairInfo {
    int pair_id;
    int first_lot_id;
    int second_lot_id;
    string first_lot_name;
    string second_lot_name;
};
struct MatchResult {
    bool matched;
    float matched_quantity;
    bool order_fully_filled;
    float remaining_quantity;
    int counterparty_user_id;
    float matched_price;
    vector<pair<float, float>> executed_parts; 
    MatchResult() : matched(false), matched_quantity(0), order_fully_filled(false),remaining_quantity(0), counterparty_user_id(-1), matched_price(0.0f) {}
};

vector<Order> getOpenOrdersFromHash();
MatchResult findAndExecuteMatchingOrders(Order& newOrder);
bool updateOrderQuantity(int order_id, float new_quantity);
bool updateUserBalance(int user_id, int lot_id, float amount);
float getUserBalance(int user_id, int lot_id);
vector<Order> getUserOpenOrders(int user_id);
bool closeOrder(int order_id, const string& timestamp);
bool updateOrderQuantityAndClose(int order_id, float new_quantity, const string& timestamp);
Order* findMatchingOrder(Order& newOrder);
#endif
