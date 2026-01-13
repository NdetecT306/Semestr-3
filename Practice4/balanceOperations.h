#ifndef BALANCE_OPERATIONS_H
#define BALANCE_OPERATIONS_H
#include "monitor.h"
#include <string>
#include "userOperations.h"
#include "trading.h"
#include "MYCSV.h"
#include "struct.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include<fstream>
using namespace std;
// Функции для работы с балансом
struct PairInfo;
PairInfo getPairInfo(int pair_id);
bool checkUserBalanceForOrder(const string& userKey, int pair_id, float quantity, float price, const string& type);
bool reserveFundsForOrder(int user_id, int pair_id, float quantity, float price, const string& type);
bool returnReservedFunds(int user_id, int pair_id, float quantity, float price, const string& type);
bool processOrderExecution(int buyer_id, int seller_id, int pair_id,float quantity, float execution_price,float original_price = 0.0f);
bool validateOrderParameters(int pair_id, float quantity, float price, const string& type);
bool refundExcessFundsToBuyer(int buyer_id, int pair_id,float original_price, float execution_price,float executed_quantity);

#endif
