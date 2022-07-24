//
// Created by arnaudskovich on 09/07/22.
//

#ifndef INF200_SELL_H
#define INF200_SELL_H

#include "global.h"
#include "product.h"

struct SellItem {
    Product pd;
    unsigned int quantity = 0;
};

struct Sell {
    map <int, SellItem> basket;
    unsigned int moneyPaid = 0;
};

int operateNewSell(string const &shopName);

void sellHistory(string const &shopName);

void flushSellHistory(string const &shopName);

int sellManager(string const &shopName);

#endif //INF200_SELL_H
