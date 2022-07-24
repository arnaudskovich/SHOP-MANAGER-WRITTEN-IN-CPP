//
// Created by arnaudskovich on 09/07/22.
//
#include "global.h"
#include "sell.h"
#include "product.h"

int updateProductQtAfterSell(string const &shopName, string const &pdName, int const &modifierQt) {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2", hist =
            shopName + "_hist.txt";
    string tempLine;

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream stocksHist(hist, ios::app);

    if (!oldList || !updatedProductList || !stocksHist) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }

    while (getline(oldList, tempLine)) {
        Product nextPd;
        initializeProductFromProdLine(tempLine, &nextPd);
        if (nextPd.name == pdName) {
            int nextQt = (int) nextPd.qtInStock + modifierQt;
            string opMessage =
                    (modifierQt >= 0 ? " Added " : " Removed ") + to_string(modifierQt) + " on " + nextPd.name +
                    " quantity";
            nextQt = nextQt >= 0 ? nextQt : 0;
            nextPd.qtInStock = nextQt;
            updatedProductList << nextPd.name << ":::" << nextPd.description << ":::" << nextPd.price << ":::"
                               << nextPd.qtInStock << ":::" << nextPd.alertQt << endl;
            stocksHist << getDateString() << opMessage << endl;
        } else {
            updatedProductList << tempLine << endl;
        }
    }
    updatedProductList.close();
    oldList.close();
    stocksHist.close();

    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());

    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        return 1;
    }
    ErrorHandler("SOME ERRORS OCCURS WHILE UPDATING PRODUCT QUANTITY AFTER SELL! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 1;
}

static string formatSellToLine(Sell const &sell) {
    string toReturn;
    toReturn += getDateString() + "{}";
    toReturn += to_string(sell.moneyPaid) + "{}";
    for (auto const &sellItm: sell.basket) {
        string pdPart;
        pdPart += sellItm.second.pd.name;
        pdPart += ":::" + to_string(sellItm.second.pd.price);
        pdPart += "##" + to_string(sellItm.second.quantity);
        if (sellItm.first != 0) toReturn += "[]";
        toReturn += pdPart;
    }
    return toReturn;
}

static int saveSellToFile(string const &shopName, Sell const &sell) {
    if (sell.basket.empty()) return 2;
    ofstream sellFile(shopName + "_sells.txt");
    if (!sellFile) {
        ErrorHandler("COULD NOT ACCESS SELLS HISTORY FILE");
        return 0;
    }
    for (auto const &sellItm: sell.basket) {
        int mdQt = (int) sellItm.second.quantity * (-1);
        int res = updateProductQtAfterSell(shopName, sellItm.second.pd.name, mdQt);
        if (res != 1) ErrorHandler("COULD NOT UPDATE QUANTITY FOR SELL");
    }
    string lineToSave = formatSellToLine(sell);
    sellFile << lineToSave << endl;
    sellFile.close();
    DisplayInfo("SELL HAS BEEN SAVED");
    return 4;
}

static void printBasketContent(map<int, SellItem> const &sellList, int const &showTotal) {
    //will display
    //PD_NAME (price x quantity = total)
    unsigned int total = 0;
    DisplayMessage("======= Shop Basket Content ========");
    if (sellList.empty()) {
        DisplayInfo("SHOP BASKET IS EMPTY FOR THE MOMENT");
        return;
    }
    for (auto const &sell_: sellList) {
        int partial_tt = (int) (sell_.second.pd.price * sell_.second.quantity);
        string message;
        message += to_string(sell_.first + 1) + "-) ";
        message += sell_.second.pd.name;
        message += " (" + to_string(sell_.second.pd.price);
        message += " x " + to_string(sell_.second.quantity) + " = ";
        message += to_string(partial_tt) + ")";
        total += partial_tt;
        DisplayMessage(message);
    }
    if (showTotal) DisplayMessage("Total sold: " + to_string(total));
}

static void showNewSellOptions() {
    vector<string> options;
    options.emplace_back("Add a product");
    options.emplace_back("Remove a product");
    options.emplace_back("Update quantity for a product");
    options.emplace_back("Confirm sell and save");
    PrintListAndAsk(options, 1);
}

static void updateQuantityForItem(Sell &sell, string const &shopName) {
    vector<string> pdListWithQt;
    string pdId, nextQt;
    DisplayMessage("=======> ITEM QUANTITY UPDATE <=======");
    printBasketContent(sell.basket, 0);
    GET_PD_ID:
    Ask("Enter the number of the product to update or 0 to cancel: ");
    getline(cin, pdId);
    if (!isValidIntChoice(pdId, (int) sell.basket.size(), 0)) {
        DisplayInfo("INVALID CHOICE");
        goto GET_PD_ID;
    }
    if (isPrevMenu(pdId)) {
        Clear();
        return;
    }
    GET_PD_QT:
    Ask("Enter the new quantity of the product or 0 to cancel: ");
    getline(cin, nextQt);
    Product tmp;
    getProduct(shopName, sell.basket.at(stoi(pdId) - 1).pd.name, &tmp);
    if (!isValidIntChoice(nextQt, tmp.qtInStock, 0)) {
        DisplayInfo("INVALID QUANTITY SEEING THE STOCK");
        goto GET_PD_QT;
    }
    if (isPrevMenu(pdId)) {
        Clear();
        return;
    }
    sell.basket.at(stoi(pdId) - 1).quantity = stoi(nextQt);
}

static void removeItemFromBasket(Sell &sell) {
    vector<string> pdListWithQt;
    string pdId;
    if (sell.basket.empty()) {
        Clear();
        DisplayInfo("BASKET IS ALREADY EMPTY! NO PRODUCT TO REMOVE!");
        return;
    }
    DisplayMessage("=======> ITEM REMOVAL <=======");
    printBasketContent(sell.basket, 0);
    GET_PD_ID:
    Ask("Enter the number of the product to remove or 0 to cancel: ");
    getline(cin, pdId);
    if (!isValidIntChoice(pdId, (int) sell.basket.size(), 0)) {
        DisplayInfo("INVALID CHOICE");
        goto GET_PD_ID;
    }
    if (isPrevMenu(pdId)) {
        Clear();
        return;
    }
    int pos = stoi(pdId) - 1;
    int index = 0;
    map<int, SellItem> basketCopy;
    for (auto &sellItm: sell.basket) {
        if (sellItm.first != pos) {
            basketCopy[index] = sellItm.second;
            index++;
        }
    }
    sell.basket = basketCopy;
    Clear();
    DisplayMessage("PRODUCT HAS BEEN REMOVED FROM THE BASKET");
}

static void addItemToBasket(Sell &sell, string const &shopName) {
    map<int, Product> pdList;
    SellItem tempItem;
    string pdId, pdQt;
    getAllProducts(shopName, pdList);
    if (pdList.empty()) {
        DisplayInfo("NO PRODUCT FOUND IN THIS SHOP");
        return;
    }
    DisplayMessage("====== ADDING PRODUCT TO BASKET ======");
    vector<string> prodNamesAndQt;
    prodNamesAndQt.clear();
    for (auto const &pd: pdList) {
        prodNamesAndQt.emplace_back(pd.second.name + " (" + to_string(pd.second.qtInStock) + ")");
    }
    PrintList(prodNamesAndQt);
    GET_PROD_ID:
    Ask("Enter a product number or 0 to see basket content: ");
    getline(cin, pdId);
    if (!isValidIntChoice(pdId, prodNamesAndQt.size(), 0)) {
        DisplayInfo("INVALID PRODUCT NUMBER");
        goto GET_PROD_ID;
    }
    if (isPrevMenu(pdId)) {
        Clear();
        return;
    }
    tempItem.pd = pdList[stoi(pdId) - 1];
    GET_PROD_QT:
    Ask("Enter the quantity to add or 0 to see basket content: ");
    getline(cin, pdQt);
    if (!isValidIntChoice(pdQt, tempItem.pd.qtInStock, 0)) {
        DisplayInfo("INVALID PRODUCT QUANTITY CONSIDERING THE STOCK");
        goto GET_PROD_QT;
    }
    if (isPrevMenu(pdQt)) {
        Clear();
        return;
    }
    tempItem.quantity = stoi(pdQt);
    int added = 0;
    for (auto &sellItem: sell.basket) {
        if (sellItem.second.pd.name == tempItem.pd.name) {
            int tempQt = (int) (sellItem.second.quantity + tempItem.quantity);
            if (!isValidIntChoice(to_string(tempQt), tempItem.pd.qtInStock, 0)) {
                int left = (int) (tempItem.pd.qtInStock - sellItem.second.quantity);
                DisplayInfo("INVALID PRODUCT QUANTITY CONSIDERING THE STOCK (current " + to_string(left) + ")");
                goto GET_PROD_QT;
            }
            sellItem.second.quantity = tempQt;
            added = 1;
            break;
        }
    }
    if (added == 0)sell.basket[(int) sell.basket.size()] = tempItem;
    Clear();
    DisplayMessage("PRODUCT ADDED TO BASKET");
}

static int operateNewSellDispatcher(string const &shopName, Sell &sell, int const &action) {
    int doneState = 0;
    switch (action) {
        case 1:
            addItemToBasket(sell, shopName);
            doneState = 1;
            break;
        case 2:
            removeItemFromBasket(sell);
            doneState = 1;
            break;
        case 3:
            updateQuantityForItem(sell, shopName);
            doneState = 1;
            break;
        case 4:
            doneState = saveSellToFile(shopName, sell);
            break;
        default:
            break;
    }
    return doneState;
}

int operateNewSell(string const &shopName) {
    Sell newSell;
    string sellOptChoice, cancelOpt;
    int state, choice;
    START:
    DisplayMessage("=====> NEW SELL " + shopName + " <=====");
    printBasketContent(newSell.basket, 1);
    showNewSellOptions();
    getline(cin, sellOptChoice);

    if (!isValidIntChoice(sellOptChoice, 4, 0)) {
        Clear();
        DisplayInfo("INVALID CHOICE");
        goto START;
    }

    if (isPrevMenu(sellOptChoice)) {
        Ask("Do you really want to cancel the current sell operation ? enter 1 to confirm: ");
        getline(cin, cancelOpt);
        Clear();
        if (cancelOpt == "1") return 2;
        goto START;
    }

    Clear();

    choice = stoi(sellOptChoice);
    state = operateNewSellDispatcher(shopName, newSell, choice);
    if (state == 4) return 1;
    if (state != 0) goto START;
    return 0;
}

static int isRefresh(string const &choice) {
    return (choice == "r" || choice == "R");
}

static void parseAndPrintSellLine(string const &sellLine, unsigned int const &pos) {
    //A line will look like
    //Date{}MoneyPaid{}Product1:::price1##qt1[]Product2:::price2##qt2[]ProductX:::priceX##qtX
    vector<string> mainParts = splitStr(sellLine, "{}");
    string dateStr = mainParts[0];
    string moneyPaid = mainParts[1];
    string soldProducts = mainParts[2];
    vector<string> pdListAndQt = splitStr(soldProducts, "[]");
    map<int, SellItem> sellList;
    int count = 0;
    for (auto const &sell: pdListAndQt) {
        SellItem tmpSellItem;
        Product tmp_pd;
        vector<string> tmpParts = splitStr(sell, "##");
        vector<string> pdParts = splitStr(tmpParts[0], ":::");
        initializeProduct(pdParts[0], "", stoi(pdParts[1]), 0, 0, &tmp_pd);
        tmpSellItem.pd = tmp_pd;
        tmpSellItem.quantity = stoi(tmpParts[1]);
        sellList[count] = tmpSellItem;
        count++;
    }

    Blank();
    DisplayMessage("================= SELL N° " + to_string(pos) + " ===================");
    //SHOP BASKET CONTENT
    printBasketContent(sellList, 1);
    DisplayMessage("Amount paid: " + moneyPaid);
    DisplayMessage("Date: " + dateStr);
    DisplayMessage("============== ========= ======== ============");
    Blank();

}

void sellHistory(string const &shopName) {
    string const &shopHistory = shopName + "_sells.txt";
    string choice, tmpLine;
    START:
    ifstream sellHst(shopHistory);
    if (!sellHst) ErrorHandler("COULD NOT ACCESS REQUIRED FILE");
    DisplayMessage("===========> SELL HISTORY FOR " + shopName + " <==========");
    int pos = 0;
    while (getline(sellHst, tmpLine)) {
        pos++;
        parseAndPrintSellLine(tmpLine, pos);
    }
    if (pos == 0) {
        DisplayInfo("NO HISTORY FOUND IN THIS SHOP");
    }
    Ask("Enter r to refresh or anything else to quit: ");
    getline(cin, choice);
    Clear();
    if (isRefresh(choice)) goto START;
}

void flushSellHistory(string const &shopName) {
    string const sellName = shopName + "_sells.txt";
    int const sellHst = remove(sellName.c_str());
    if (sellHst != 0) ErrorHandler("COULD NOT ACCESS REQUIRED FILES");
    ofstream newHst(sellName);
    if (!newHst) ErrorHandler("COULD NOT CREATE NEW HISTORY FILE");
    DisplayInfo("SHOP SELL HISTORY HAS BEEN FLUSHED");
}

static void showSellManagerOptions(string const &shopName) {
    vector<string> options;
    options.emplace_back("Operate a new sell");
    options.emplace_back("See sell history");
    options.emplace_back("Flush sell history");
    DisplayMessage("=====> SELL MANAGER FOR " + shopName + " <======");
    PrintListAndAsk(options, 1);
}

static int dispatchSellActions(string const &shopName, int const action) {
    int doneState = 0;
    switch (action) {
        case 1:
            doneState = operateNewSell(shopName);
            break;
        case 2:
            sellHistory(shopName);
            doneState = 1;
            break;
        case 3:
            flushSellHistory(shopName);
            doneState = 1;
            break;
        default:
            break;
    }
    return doneState;
}

int sellManager(string const &shopName) {
    unsigned short choice, doneState;
    string choiceStr;
    START:
    showSellManagerOptions(shopName);
    getline(cin, choiceStr);
    Clear();
    if (!isValidIntChoice(choiceStr, 3, 0)) {
        DisplayError("INVALID CHOICE");
        goto START;
    }
    if (isPrevMenu(choiceStr)) return 2;
    choice = stoi(choiceStr);
    doneState = dispatchSellActions(shopName, choice);
    if (doneState != 0) goto START;
    return 0;
}