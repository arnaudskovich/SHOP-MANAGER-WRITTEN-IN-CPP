//
// Created by arnaudskovich on 09/07/22.
//

#ifndef INF200_STOCKS_H
#define INF200_STOCKS_H

#include "global.h"

int addNewStock(string const &shopName);

void getStockOverView(string const &shopName);

int updateProductQt(string const &shopName, int const &pos, int const &modifierQt);

void getFinishingProduct(string const &shopName);

void flushHistory(string const &shopName);

void showStockOptions(string const& shopName);

int stockManager(string const &shopName);

#endif //INF200_STOCKS_H
