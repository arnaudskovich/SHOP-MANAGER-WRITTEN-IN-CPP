//
// Created by arnaudskovich on 09/07/22.
//
#include "global.h"
#include "shops.h"
#include "sell.h"
#include "stocks.h"
#include "product.h"

static void initializeShopsFile() {
    ofstream shops("shops.txt", ios::app);
    if (!shops) ErrorHandler("COULD NOT INITIALIZE NECESSARY FILES");
}

static int dispatchShopAction(unsigned short action, string &shopName) {
    int done = 0;
    switch (action) {
        case 1:
            done = sellManager(shopName);
            break;
        case 2:
            done = stockManager(shopName);
            break;
        case 3:
            done = productManager(shopName);
            break;
        case 4:
            done = renameShop(shopName);
            break;
        default:
            break;
    }
    return done;
}

int shopManager(string &activeShopName) {
    initializeShopsFile();
    string shopRootMenuChoiceStr;
    int actionState, _result;
    unsigned short shopRootMenuChoice;
    SHOP_SELECT_SETTINGS:
    setActiveShop(activeShopName);
    Clear();
    SHOP_ROOT_MENU:
    showShopOptions(activeShopName);
    getline(cin, shopRootMenuChoiceStr);
    Clear();
    if (!isValidIntChoice(shopRootMenuChoiceStr, 7, 1)) {
        DisplayError("INVALID CHOICE, ENTER A DIGIT BETWEEN 1 AND 7 INCLUDED");
        goto SHOP_ROOT_MENU;
    }
    shopRootMenuChoice = stoi(shopRootMenuChoiceStr);
    switch (shopRootMenuChoice) {
        case 5:
            _result = deleteShop(activeShopName);
            if (_result) {
                if (_result == 2) goto SHOP_ROOT_MENU;
                goto SHOP_SELECT_SETTINGS;
            }
            ErrorHandler("COULD NOT EXECUTE DELETION FOR CURRENT SHOP");
            break;
        case 6:
            goto SHOP_SELECT_SETTINGS;
        case 7:
            QUIT();
            break;
        default:
            actionState = dispatchShopAction(shopRootMenuChoice, activeShopName);
            if (actionState == 2) goto SHOP_ROOT_MENU;
            if (!actionState) DisplayError("ACTION DID NOT COMPLETE");
            goto SHOP_ROOT_MENU;
    }
    return 0;
}

int isValidShopName(string const &name) {
    regex const validShopName("[a-zA-Z0-9 _'-]+");
    int const min = 3, max = 60;
    int const l = (int) name.size();
    return (l >= min && l <= max && regex_match(name, validShopName));
}

void showShopOptions(string const &name) {
    vector<string> options;
    options.emplace_back("Sell Manager");
    options.emplace_back("Stock Manager");
    options.emplace_back("Product Manager");
    options.emplace_back("Rename shop");
    options.emplace_back("Delete shop");
    options.emplace_back("Change shop");
    options.emplace_back("Quit program");
    string tempStr = "=======> " + name + " <=======";
    DisplayMessage(tempStr);
    PrintListAndAsk(options, 0);
}

void getOldShops(vector<string> &shops) {
    string temp_name;
    shops.clear();
    ifstream savedShops("shops.txt");
    if (!savedShops)ErrorHandler("COULD NOT ACCESS OLD SHOPS");
    while (getline(savedShops, temp_name)) {
        shops.push_back(temp_name);
    }
}

int createShop(string const &name) {
    ofstream shops_out("shops.txt", ios::app);
    ifstream shops_in("shops.txt");
    if (!shops_out || !shops_in) {
        ErrorHandler("COULD NOT ACCESS SHOP FILES");
        return 1;
    }
    unsigned short const &length = name.size();
    string temp;
    if (isValidShopName(name)) {
        while (getline(shops_in, temp)) {
            if (temp == name) {
                DisplayError("A SHOP ALREADY EXISTS WITH THE SAME NAME");
                return 0;
            }
        }
        shops_out << name << endl;
        ofstream sells(name + "_sells.txt");
        ofstream stocks(name + "_stocks.txt");
        ofstream hst(name + "_hist.txt");
        if (stocks && sells && hst) {
            sells.close();
            stocks.close();
            hst.close();
            return 1;
        } else {
            Clear();
            ErrorHandler("COULD NOT INITIALIZE REQUIRED FILES");
        }
    }
    shops_out.close();
    shops_in.close();
    return 0;
}

int setActiveShop(string &name) {
    string shop_name, selectedOldShopStr, choiceStr;
    unsigned short done = 1, choice;
    unsigned int selectedOldShopNum, oldShopsCount;
    vector<string> shops;
    vector<string> blank;
    vector<string> props;
    props.emplace_back("Restore a shop");
    props.emplace_back("Create a new shop");
    props.emplace_back("Quit");
    SET_SHOP_START:
    DisplayMessage("=====> SHOP MANAGER <=====");
    PrintListAndAsk(props, 0);
    getline(cin, choiceStr);
    if (!isValidIntChoice(choiceStr, 3, 1)) {
        Clear();
        goto SET_SHOP_START;
    }
    choice = stoi(choiceStr);
    Clear();
    if (choice == 3) {
        QUIT();
    } else if (choice == 1) {
        getOldShops(shops);
        oldShopsCount = shops.size();
        SELECT_SHOP:
        if (oldShopsCount == 0) {
            Clear();
            DisplayInfo("NO OLD SHOP SAVE FOUND");
            goto SET_SHOP_START;
        }
        DisplayMessage("=========> SHOPS LIST <=========");
        PrintListAndAsk(shops, 1);
        getline(cin, selectedOldShopStr);
        Clear();
        if (isPrevMenu(selectedOldShopStr)) goto SET_SHOP_START;
        if (!isValidIntChoice(selectedOldShopStr, oldShopsCount, 1)) goto SELECT_SHOP;
        selectedOldShopNum = stoi(selectedOldShopStr) - 1;
        //TODO OBSERVE CAREFULLY
        name = shops[selectedOldShopNum];
    } else if (choice == 2) {
        CREATE_SHOP:
        Ask("Enter a shop name (3-60 long, only a-z, A-Z, 0-9, -, _ and space are authorized) or 0 to cancel: ");
        getline(cin, shop_name);
        Clear();
        if (isPrevMenu(shop_name)) {
            goto SET_SHOP_START;
        }
        if (!createShop(shop_name)) {
            DisplayError("INVALID SHOP NAME FORMAT");
            goto CREATE_SHOP;
        }
        name = shop_name;
        DisplayInfo(name + " HAS BEEN SUCCESSFULLY CREATED");
    } else {
        Clear();
        DisplayError("INVALID CHOICE");
        goto SET_SHOP_START;
    }
    return done;
}

int deleteShop(string const &name) {
    vector<string> shops;
    string tmp, confirmDeletionStr;
    int done = 0;
    Clear();
    Ask("DO YOU REALLY WANT TO DELETE '" + name + "' ? Enter 1 to confirm or other thing to cancel: ");
    getline(cin, confirmDeletionStr);
    Clear();
    if (confirmDeletionStr != "1") {
        DisplayInfo("DELETION HAS BEEN CANCELED");
        return 2;
    }
    ofstream nextShops("shops.txt.del");
    ifstream currShops("shops.txt");
    getOldShops(shops);
    while (getline(currShops, tmp)) {
        if (tmp == name) {
            string const _stocks = tmp + "_stocks.txt";
            string const _sells = tmp + "_sells.txt";
            string const _hst = tmp + "_hist.txt";
            int state_st = remove(_stocks.c_str());
            int state_sl = remove(_sells.c_str());
            int state_hst = remove(_hst.c_str());
            if (state_sl == 0 && state_st == 0 && state_hst == 0) done = 1;
            else {
                nextShops.close();
                currShops.close();
                remove("shops.txt.del");
                done = 0;
                DisplayError("DATA ERROR MAY OCCUR");
                ErrorHandler("COULD NOT DELETE SPECIFIED SHOP");
            }
            continue;
        }
        nextShops << tmp << endl;
    }
    int const finalRemove = remove("shops.txt");
    int const finalRename = rename("shops.txt.del", "shops.txt");
    done = (done && (finalRemove == 0 && finalRename == 0));
    if (done) DisplayInfo("SHOP '" + name + "' HAS BEEN DELETED SUCCESSFULLY");
    return done;
}

int renameShop(string &name) {
    Clear();
    vector<string> shops;
    string temp;
    ofstream nextShop("shops.txt.upd");
    if (!nextShop) ErrorHandler("COULD NOT CREATE REQUIRED FILES");
    getOldShops(shops);
    int const numberOfShops = (int) shops.size();
    GET_UPDATED_NAME:
    Ask("Enter the new name of " + name + "  (or 0 to keep current name): ");
    getline(cin, temp);
    Clear();
    if (isPrevMenu(temp)) return 1;
    if (!isValidShopName(temp)) goto GET_UPDATED_NAME;
    for (int k = 0; k < numberOfShops; k++) {
        if (shops[k] == name) {
            nextShop << temp << endl;
            continue;
        }
        nextShop << shops[k] << endl;
    }
    int const removeState = remove("shops.txt");
    nextShop.close();
    if (removeState != 0) {
        remove("shops.txt.upd");
        ErrorHandler("COULD NOT ACCESS REQUIRED FILES");
    }
    int const renameState = rename("shops.txt.upd", "shops.txt");
    string const prevSells = name + "_sells.txt";
    string const nextSells = temp + "_sells.txt";
    string const prevStocks = name + "_stocks.txt";
    string const nextStocks = temp + "_stocks.txt";
    string const prevHst = name + "_hist.txt";
    string const nextHst = temp + "_hist.txt";
    int const renameSells = rename(prevSells.c_str(), nextSells.c_str());
    int const renameStocks = rename(prevStocks.c_str(), nextStocks.c_str());
    int const renameHst = rename(prevHst.c_str(), nextHst.c_str());
    cout << renameState << renameSells << renameStocks << endl;
    int const updateState = renameState == 0 && renameSells == 0 && renameStocks == 0 && renameHst == 0;
    if (updateState) {
        DisplayInfo("'" + name + "' HAS BEEN RENAMED '" + temp + "'");
        name = temp;
    } else {
        ErrorHandler("COULD NOT RENAME SHOP! FATAL DATA CORRUPTION MIGHT OCCUR");
    }
    return updateState;
}