//
// Created by arnaudskovich on 09/07/22.
//
#include "global.h"
#include "stocks.h"
#include "product.h"

static int isCancel(string const &input) {
    return (input == "c" || input == "C");
}

static int isRefresh(string const &choice) {
    return (choice == "r" || choice == "R");
}

static int isValidQuantity(string const &qt) {
    regex const pdQt("-?[0-9]+");
    if (regex_match(qt, pdQt)) {
        return 1;
    }
    return 0;
}

static void printOverviewLine(Product const &pd, int const &pos) {
    cout << endl << pos << "-) Product name: " << pd.name << " <======> Quantity in stock: " << pd.qtInStock << endl;
}

int updateProductQt(string const &shopName, int const &pos, int const &modifierQt) {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2", hist =
            shopName + "_hist.txt";
    string tempLine;
    unsigned int currentLine = 1;

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream stocksHist(hist, ios::app);

    if (!oldList || !updatedProductList || !stocksHist) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }

    while (getline(oldList, tempLine)) {
        if (currentLine == pos) {
            Product nextPd;
            initializeProductFromProdLine(tempLine, &nextPd);
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
        currentLine++;
    }
    updatedProductList.close();
    oldList.close();
    stocksHist.close();

    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());

    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        DisplayInfo("PRODUCT QUANTITY HAS BEEN UPDATED");
        return 1;
    }

    ErrorHandler("SOME ERRORS OCCURS WHILE UPDATING PRODUCT QUANTITY! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 1;

}

int addNewStock(string const &shopName) {
    map<int, Product> pdList;
    vector<string> pdNames;
    string pdId, quantity;
    START:
    pdNames.clear();
    getAllProducts(shopName, pdList);
    for (auto const &pd: pdList) {
        pdNames.emplace_back(pd.second.name + " ( " + to_string(pd.second.qtInStock) + " )");
    }
    DisplayMessage(" =====> ADD OR REMOVE PRODUCT QUANTITY MENU <=====");
    PrintList(pdNames);
    if (pdList.empty()) {
        DisplayInfo("No Product found in this shop. Start adding one");
        Ask("Enter anything to go back: ");
        getline(cin, pdId);
        Clear();
        return 2;
    }
    GET_ID:
    Ask("Enter a product number to add or remove a quantity (enter C to cancel): ");
    getline(cin, pdId);
    if (isCancel(pdId)) {
        Clear();
        return 2;
    }
    if (!isValidIntChoice(pdId, pdNames.size(), 1)) {
        DisplayError("INVALID PRODUCT NUMBER");
        goto GET_ID;
    }
    GET_QT:
    Ask("Enter quantity to add (positive number) or remove (negative number) (enter C to cancel): ");
    getline(cin, quantity);
    if (isCancel(quantity)) {
        Clear();
        goto START;
    }
    if (isValidQuantity(quantity) != 1) {
        DisplayError("INVALID QUANTITY");
        goto GET_QT;
    }

    if (updateProductQt(shopName, stoi(pdId), stoi(quantity)) == 1) {
        Clear();
        DisplayInfo("PRODUCT QUANTITY UPDATED");
        goto START;
    }
    DisplayError("COULD NOT UPDATE STOCK");
    return 1;
}

void getStockOverView(string const &shopName) {
    string choice;
    START:
    DisplayMessage("=========> " + shopName + " STOCK OVERVIEW <=======");
    map<int, Product> pdList;
    getAllProducts(shopName, pdList);
    for (auto const &pd: pdList) {
        printOverviewLine(pd.second, pd.first + 1);
    }
    Ask("Enter r to refresh or anything else to quit overview: ");
    getline(cin, choice);
    Clear();
    if (isRefresh(choice)) {
        goto START;
    }
}

void getFinishingProduct(string const &shopName) {
    string choice;
    map<int, Product> pdList;
    START:
    getAllProducts(shopName, pdList);
    int shown = 0;
    for (auto const &pd: pdList) {
        if (pd.second.qtInStock <= pd.second.alertQt) {
            printOverviewLine(pd.second, pd.first + 1);
            shown++;
        }
    }
    if (shown == 0) DisplayInfo("NO PRODUCT IN ALERT ZONE");
    Ask("Enter r to refresh or anything else to quit overview: ");
    getline(cin, choice);
    Clear();
    if (isRefresh(choice)) {
        goto START;
    }
}

void showStockOptions(string const &shopName) {
    vector<string> options;
    options.emplace_back("Add a quantity or remove a quantity of product");
    options.emplace_back("Get overview of current state");
    options.emplace_back("Get stock history");
    options.emplace_back("Get products in alert zone");
    options.emplace_back("Flush history");
    DisplayMessage("======> STOCK MANAGER FOR " + shopName + " <=======");
    PrintListAndAsk(options, 1);
}

void showStockHistory(string const &shopName) {
    string const hst_name = shopName + "_hist.txt";
    string choice, tmpLine;
    START:
    vector<string> history;
    ifstream hstFile(hst_name);
    if (!hstFile) ErrorHandler("COULD NOT ACCESS REQUIRED FILES");
    DisplayMessage("========> " + shopName + " HISTORY <========");
    int pos = 0;
    while (getline(hstFile, tmpLine)) {
        pos++;
        DisplayMessage(tmpLine);
    }
    if (pos == 0) {
        DisplayInfo("NO HISTORY FOUND IN THIS SHOP");
    }
    Ask("Enter R to refresh (anything else to quit): ");
    getline(cin, choice);
    Clear();
    if (isRefresh(choice)) goto START;
}

void flushHistory(string const &shopName) {
    string const hstName = shopName + "_hist.txt";
    int const delOldHst = remove(hstName.c_str());
    if (delOldHst != 0) ErrorHandler("COULD NOT ACCESS REQUIRED FILES");
    ofstream newHst(hstName);
    if (!newHst) ErrorHandler("COULD NOT CREATE NEW HISTORY FILE");
    DisplayInfo("SHOP STOCKS HISTORY HAS BEEN FLUSHED");
}

int stockActionsDispatcher(int const &action, string const &shopName) {
    int doneState = 0;
    switch (action) {
        case 1:
            doneState = addNewStock(shopName);
            break;
        case 2:
            getStockOverView(shopName);
            doneState = 1;
            break;
        case 3:
            showStockHistory(shopName);
            doneState = 1;
            break;
        case 4:
            getFinishingProduct(shopName);
            doneState = 1;
            break;
        case 5:
            flushHistory(shopName);
            doneState = 1;
            break;
        default:
            break;
    }
    return doneState;
}

int stockManager(string const &shopName) {
    int currentChoice, doneState;
    string choiceStr;
    START:
    showStockOptions(shopName);
    getline(cin, choiceStr);
    Clear();
    if (!isValidIntChoice(choiceStr, 5, 0)) {
        DisplayInfo("INVALID CHOICE");
        goto START;
    }
    if (isPrevMenu(choiceStr)) return 2;
    currentChoice = stoi(choiceStr);
    doneState = stockActionsDispatcher(currentChoice, shopName);
    if (doneState != 0) goto START;
    return 0;
}