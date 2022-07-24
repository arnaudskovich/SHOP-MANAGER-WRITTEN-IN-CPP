//
// Created by arnaudskovich on 09/07/22.
//

#ifndef INF200_SHOPS_H
#define INF200_SHOPS_H

#include "global.h"

int shopManager(string &activeShopName);

int isValidShopName(string const &name);

int setActiveShop(string &name);

int createShop(string const &name);

int renameShop(string &name);

int deleteShop(string const &name);

void getOldShops(vector<string> &shops);

void showShopOptions(string const &name);


#endif //INF200_SHOPS_H
