//
// Created by arnaudskovich on 10/07/22.
//
#include "global.h"
#include "product.h"

void initializeProduct(string const &tmpName, string const &tmpDescription, unsigned int const &tmpPrice,
                       unsigned int const &tmpQt, unsigned int const &alertQt, Product *pd) {
    pd->name = tmpName;
    pd->description = tmpDescription;
    pd->price = tmpPrice;
    pd->qtInStock = tmpQt;
    pd->alertQt = alertQt;
}

void initializeProductFromProdLine(string const &prodLine, Product *pd) {
    vector<string> prodParts = splitStr(prodLine, ":::");
    initializeProduct(prodParts[0], prodParts[1], stoi(prodParts[2]), stoi(prodParts[3]), stoi(prodParts[4]), pd);
}

static int isUniqueProduct(string const &shopName, string const &productName) {
    ifstream stocksFile(shopName + "_stocks.txt");
    string currentLine;
    while (getline(stocksFile, currentLine)) {
        if ((int) currentLine.find(productName) == 0) {
            stocksFile.close();
            return 0;
        }
    }
    stocksFile.close();
    return 1;
}

static int isCancel(string const &input) {
    return (input == "c" || input == "C");
}

static int Cancel() {
    Clear();
    return 2;
}

int isValidProductName(string const &productName) {
    regex const validProductName("[a-zA-Z0-9 _'-]+");
    int const min = 3;
    int const max = 60;
    int const l = (int) productName.size();
    return (l >= min && l <= max && regex_match(productName, validProductName));
}

static int isValidProductDescription(string const &description) {
    regex const validProductDescription("[a-zA-Z0-9 _'-]+");
    int const min = 3;
    int const max = 255;
    int const l = (int) description.size();
    return (l >= min && l <= max && regex_match(description, validProductDescription));
}

void printProduct(Product const &pd, string const &pos) {
    DisplayMessage("======= PRODUCT INFOS =======");
    DisplayMessage("PRODUCT NUMBER: " + pos);
    DisplayMessage("PRODUCT NAME: " + pd.name);
    DisplayMessage("PRODUCT DESCRIPTION: " + pd.description);
    DisplayMessage("PRODUCT PRICE: " + to_string(pd.price));
    DisplayMessage("QUANTITY IN STOCK: " + to_string(pd.qtInStock));
    DisplayMessage("ALERT QUANTITY: " + to_string(pd.alertQt));
    DisplayMessage("=============================");
}

void showEmbeddedProductActions(unsigned short const &showPrevList) {
    vector<string> options;
    string message = "Enter 1-4";
    options.emplace_back("Update product");
    options.emplace_back("Delete Product");
    options.emplace_back("Goto product menu (previous menu)");
    options.emplace_back("Goto shop menu");
    PrintList(options);
    message += showPrevList ? " (L to show the previous list): " : ": ";
    Ask(message);
}

void showProductManagerOptions(string const &shopName) {
    vector<string> options;
    options.emplace_back("List all products");
    options.emplace_back("Get a product infos by its number");
    options.emplace_back("Get a product infos by its name");
    options.emplace_back("Search product by name");
    options.emplace_back("Add new product");
    options.emplace_back("Update a product by its number");
    options.emplace_back("Update a product by its name");
    options.emplace_back("Remove a product by its number");
    options.emplace_back("Remove a product by its name");
    DisplayMessage("===> PRODUCT MANAGER FOR " + shopName + " <===");
    PrintListAndAsk(options, 1);
}

static int dispatchProductActions(unsigned short action, string const &shopName) {
    int doneState = 0;
    switch (action) {
        case 1:
            doneState = showAllProducts(shopName);
            break;
        case 2:
            doneState = getProduct(shopName, 0, nullptr);
            break;
        case 3:
            doneState = getProduct(shopName, "", nullptr);
            break;
        case 4:
            doneState = searchProduct(shopName, "");
            break;
        case 5:
            doneState = addProduct(shopName);
            break;
        case 6:
            doneState = updateProduct(shopName, 0);
            break;
        case 7:
            doneState = updateProduct(shopName, "");
            break;
        case 8:
            doneState = removeProduct(shopName, 0);
            break;
        case 9:
            doneState = removeProduct(shopName, "");
            break;
        default:
            break;
    }
    return doneState;
}

static int getProductFromInput(Product &pd, Product *oldPd = nullptr, string const &headerMessage = "",
                               string const &shopName = "") {
    Product tmpPd;
    string e = oldPd == nullptr ? ": " : " (Leave blank to conserve old value): ";
    string name, description, price, baseQt, alertQt;
    DisplayMessage(headerMessage);
    GET_PRODUCT_NAME:
    Ask("Product name (3-60 characters long) (a-z, A-Z, 0-9, _, ', - and space are authorized) (enter C to cancel)" +
        e);
    getline(cin, name);
    if (isCancel(name)) return Cancel();
    if (name.empty() && oldPd != nullptr) {
        name = oldPd->name;
        goto GET_PRODUCT_DESCRIPTION;
    }
    if (!isValidProductName(name)) {
        DisplayError("PRODUCT'S NAME DOES NOT RESPECT THE SPECIFIED FORMAT");
        goto GET_PRODUCT_NAME;
    }
    if (!isUniqueProduct(shopName, name)) {
        DisplayError("A PRODUCT ALREADY EXISTS WITH THE SAME NAME");
        goto GET_PRODUCT_NAME;
    }

    GET_PRODUCT_DESCRIPTION:
    Ask("Product description (3-255 characters long)(same characters authorized) (enter C to cancel)" + e);
    getline(cin, description);
    if (isCancel(description)) return Cancel();
    if (description.empty() && oldPd != nullptr) {
        description = oldPd->description;
        goto GET_PRODUCT_PRICE;
    }
    if (!isValidProductDescription(description)) {
        DisplayError("PRODUCT'S DESCRIPTION INCLUDED UNAUTHORIZED CHARACTERS");
        goto GET_PRODUCT_DESCRIPTION;
    }

    GET_PRODUCT_PRICE:
    Ask("Product price (min 1, max 4.000.000.000) (enter C to cancel)" + e);
    getline(cin, price);
    if (price.empty() && oldPd != nullptr) {
        price = to_string(oldPd->price);
        goto GET_PRODUCT_BASE_QT;
    }
    if (isCancel(price)) return Cancel();
    if (!isValidIntChoice(price, 4000000000, 1)) {
        DisplayError("PRODUCT'S PRICE IS NOT IN SPECIFIED RANGE");
        goto GET_PRODUCT_PRICE;
    }

    GET_PRODUCT_BASE_QT:
    if (oldPd == nullptr) {
        Ask("Product quantity in stock (same as price) (enter C to cancel)" + e);
        getline(cin, baseQt);
        if (isCancel(baseQt)) return Cancel();
        if (!isValidIntChoice(baseQt, 4000000000, 1)) {
            DisplayError("QUANTITY IS NOT IN SPECIFIED RANGE");
            goto GET_PRODUCT_BASE_QT;
        }
    } else {
        baseQt = to_string(oldPd->qtInStock);
        goto GET_PRODUCT_ALERT_QT;
    }
    GET_PRODUCT_ALERT_QT:
    Ask("Product alert quantity: (same as price) (enter C to cancel)" + e);
    getline(cin, alertQt);
    if (alertQt.empty() && oldPd != nullptr) {
        alertQt = to_string(oldPd->alertQt);
    }
    if (isCancel(baseQt)) return Cancel();
    if (!isValidIntChoice(baseQt, 4000000000, 0)) {
        DisplayError("QUANTITY IS NOT IN SPECIFIED RANGE");
        goto GET_PRODUCT_ALERT_QT;
    }
    pd.name = name;
    pd.description = description;
    pd.price = stoi(price);
    pd.qtInStock = stoi(baseQt);
    pd.alertQt = stoi(alertQt);
    return 1;
}

int productManager(string const &shopName) {
    unsigned short currentChoice, doneState;
    string currentChoiceStr;
    PRODUCT_MANAGER_ROOT:
    showProductManagerOptions(shopName);
    getline(cin, currentChoiceStr);
    Clear();
    if (!isValidIntChoice(currentChoiceStr, 9, 0)) goto PRODUCT_MANAGER_ROOT;
    if (isPrevMenu(currentChoiceStr)) return 2;
    currentChoice = stoi(currentChoiceStr);
    doneState = dispatchProductActions(currentChoice, shopName);
    if (doneState == 3) return 1;
    if (doneState != 1 && doneState != 2)
        DisplayError("OPERATION IN PRODUCT MANAGER DID NOT COMPLETE SUCCESSFULLY");
    goto PRODUCT_MANAGER_ROOT;
}

int showAllProducts(string const &shopName) {
    map<int, Product> pdList;
    int totalNumberOfProducts;
    string l1_choice, l2_choice;
    map<int, Product>::iterator itr;
    vector<string> listToPrint;
    PRINT_PRODUCT_LIST:
    getAllProducts(shopName, pdList);
    if (pdList.empty()) {
        DisplayInfo("NO PRODUCT FOUND IN THIS SHOP");
        return 1;
    }
    for (itr = pdList.begin(); itr != pdList.end(); itr++) {
        listToPrint.emplace_back(itr->second.name);
    }
    DisplayMessage("=========> PRODUCTS LIST <==============");
    PrintList(listToPrint);
    ASK_CHOICE:
    Ask("Enter a product's number to show its info (or C to cancel): ");
    getline(cin, l1_choice);
    totalNumberOfProducts = (int) listToPrint.size();
    if (isCancel(l1_choice)) {
        Clear();
        return 2;
    }
    if (!isValidIntChoice(l1_choice, totalNumberOfProducts, 1)) {
        DisplayInfo("INVALID CHOICE");
        goto ASK_CHOICE;
    }
    PRINT_PRODUCT:
    Clear();
    printProduct(pdList[stoi(l1_choice) - 1], l1_choice);
    ASK_EMBEDDED_ACTION:
    showEmbeddedProductActions(1);
    getline(cin, l2_choice);
    listToPrint.clear();
    if (l2_choice == "L") {
        Clear();
        goto PRINT_PRODUCT_LIST;
    }
    if (!isValidIntChoice(l2_choice, 4, 1)) {
        DisplayInfo("INVALID CHOICE");
        goto ASK_EMBEDDED_ACTION;
    }
    if (l2_choice == "3") return Cancel();
    if (l2_choice == "4") {
        Clear();
        return 3;
    }
    if (l2_choice == "2") {
        int state = removeProduct(shopName, stoi(l1_choice));
        if (state == 2) goto PRINT_PRODUCT;
        if (state == 1) goto PRINT_PRODUCT_LIST;
        ErrorHandler("COULD NOT DELETE PRODUCT");
        return 0;
    }
    if (l2_choice == "1") {
        int state = updateProduct(shopName, stoi(l1_choice));
        switch (state) {
            case 1:
                return 1;
            case 2:
                DisplayMessage("UPDATE OPERATION CANCELED");
                goto PRINT_PRODUCT;
            default:
                ErrorHandler("COULD NOT UPDATE DATA");
                break;
        }
    }
    Clear();
    return 0;
}

void getAllProducts(string const &shopName, map<int, Product> &pdList) {
    ifstream stockFile(shopName + "_stocks.txt");
    string tmpLine;
    pdList.clear();
    int k = 0;
    while (getline(stockFile, tmpLine)) {
        Product tmpProd;
        initializeProductFromProdLine(tmpLine, &tmpProd);
        pdList.emplace(k, tmpProd);
        k++;
    }
    stockFile.close();
}

int getProduct(string const &shopName, unsigned int pos = 0, Product *pd = nullptr) {
    string pdNumber, l2_choice;
    map<int, Product> pdList;
    getAllProducts(shopName, pdList);
    if (pos <= 0) {
        GET_PRODUCT_NUMBER:
        Ask("Enter the product number (or C to cancel): ");
        getline(cin, pdNumber);
        Clear();
        if (isCancel(pdNumber)) return Cancel();
        if (!isValidIntChoice(pdNumber, (int) pdList.size(), 1)) {
            DisplayInfo("NO PRODUCT CORRESPONDING");
            goto GET_PRODUCT_NUMBER;
        }
        pos = stoi(pdNumber);
    }
    if (pd == nullptr) {
        PRINT_PRODUCT:
        Clear();
        printProduct(pdList[(int) (pos - 1)], to_string(pos));

        ASK_EMBEDDED_ACTION:
        showEmbeddedProductActions(0);
        getline(cin, l2_choice);
        if (!isValidIntChoice(l2_choice, 4, 1)) {
            DisplayInfo("INVALID CHOICE");
            goto ASK_EMBEDDED_ACTION;
        }
        Clear();
        if (l2_choice == "3") return Cancel();
        if (l2_choice == "4") {
            Clear();
            return 3;
        }
        if (l2_choice == "2") {
            int state = removeProduct(shopName, pos);
            if (state == 2) goto PRINT_PRODUCT;
            if (state == 1) return 1;
            ErrorHandler("COULD NOT DELETE PRODUCT");
            return 0;
        }
        if (l2_choice == "1") {
            int state = updateProduct(shopName, pos);
            switch (state) {
                case 1:
                    return 1;
                case 2:
                    DisplayMessage("UPDATE OPERATION CANCELED");
                    goto PRINT_PRODUCT;
                default:
                    ErrorHandler("COULD NOT UPDATE DATA");
                    break;
            }
        }
        Clear();
    } else {
        *pd = pdList[(int) (pos - 1)];
    }
    Clear();
    return 1;
}

int getProduct(string const &shopName, string name = "", Product *pd = nullptr) {
    string fakeStr, l2_choice;
    unsigned int name_found = 0;
    map<int, Product> pdList;
    map<int, Product>::iterator itr;
    getAllProducts(shopName, pdList);
    if (!name.empty()) {
        goto START_ANALYSIS;
    }
    GET_PRODUCT_NAME:
    Ask("Enter the product name (or C to cancel): ");
    getline(cin, name);
    Clear();
    if (isCancel(name)) return Cancel();
    if (!isValidProductName(name)) goto GET_PRODUCT_NAME;
    START_ANALYSIS:
    for (itr = pdList.begin(); itr != pdList.end(); itr++) {
        if (itr->second.name == name) {
            name_found = 1;
            if (pd == nullptr) {
                PRINT_PRODUCT:
                Clear();
                printProduct(itr->second, to_string(itr->first + 1));
                ASK_EMBEDDED_ACTION:
                showEmbeddedProductActions(0);
                getline(cin, l2_choice);
                if (!isValidIntChoice(l2_choice, 4, 1)) {
                    DisplayInfo("INVALID CHOICE");
                    goto ASK_EMBEDDED_ACTION;
                }
                Clear();
                if (l2_choice == "3") return Cancel();
                if (l2_choice == "4") {
                    Clear();
                    return 3;
                }
                if (l2_choice == "2") {
                    int state = removeProduct(shopName, itr->second.name);
                    if (state == 2) goto PRINT_PRODUCT;
                    if (state == 1) return 1;
                    ErrorHandler("COULD NOT DELETE PRODUCT");
                    return 0;
                }
                if (l2_choice == "1") {
                    int state = updateProduct(shopName, itr->second.name);
                    switch (state) {
                        case 1:
                            return 1;
                        case 2:
                            DisplayMessage("UPDATE OPERATION CANCELED");
                            goto PRINT_PRODUCT;
                        default:
                            ErrorHandler("COULD NOT UPDATE DATA");
                            break;
                    }
                }
                Clear();
            } else {
                *pd = itr->second;
            }
            break;
        }
    }
    if (!name_found) {
        DisplayInfo("NO PRODUCT FOUND WITH THIS NAME");
        goto GET_PRODUCT_NAME;
    }
    Clear();
    return 1;
}

int searchProduct(string const &shopName, string word = "") {
    map<int, Product> pdList;
    map<int, Product> results;
    map<int, Product>::iterator itr;
    string temp;
    if (!word.empty()) {
        goto START_ANALYSIS;
    }
    GET_PRODUCT_NAME:
    Ask("Enter the word to search for (or & to cancel): ");
    getline(cin, word);
    Clear();
    if (word == "&") return Cancel();
    if (word.empty()) {
        goto GET_PRODUCT_NAME;
    }
    START_ANALYSIS:
    Clear();
    getAllProducts(shopName, pdList);
    for (itr = pdList.begin(); itr != pdList.end(); itr++) {
        if (strToLower(itr->second.name).find(strToLower(word)) != string::npos ||
            strToLower(itr->second.description).find(strToLower(word)) != string::npos) {
            results.insert(*itr);
        }
    }
    if ((int) results.size() == 0) {
        DisplayInfo("NO PRODUCT FOUND MATCHED THIS WORD");
        goto GET_PRODUCT_NAME;
    }
    DisplayMessage(" =====> RESULTS FOR YOUR SEARCH (" + word + ") <=====");
    for (itr = results.begin(); itr != results.end(); itr++) {
        printProduct(itr->second, to_string(itr->first + 1));
    }
    results.clear();
    Ask("Enter another word to search for or & to quit: ");
    getline(cin, word);
    Clear();
    if (word != "&") goto START_ANALYSIS;
    return 1;
}

/*int searchProduct(string const &shopName, map<int, Product> &container, string const &word) {
    DisplayMessage("SEARCH DONE");
    return 1;
}
*/

int addProduct(string const &shopName) {
    ofstream stockFile(shopName + "_stocks.txt", ios::app);
    ofstream hstFile(shopName + "_hist.txt", ios::app);
    if (!stockFile || !hstFile) {
        ErrorHandler("COULD NOT CREATE REQUIRED FILES FOR ADDING PRODUCT");
        return 0;
    }
    Product pd;
    int getPdState = getProductFromInput(pd, nullptr, "FILL INFORMATION ABOUT YOUR PRODUCT", shopName);
    if (getPdState == 2) return Cancel();
    stockFile << pd.name << ":::" << pd.description << ":::" << pd.price << ":::" << pd.qtInStock << ":::" << pd.alertQt
              << endl;
    hstFile << getDateString() << " Added " << pd.qtInStock << " " << pd.name << " to stock" << endl;
    stockFile.close();
    hstFile.close();
    Clear();
    DisplayInfo("'" + pd.name + "' HAS BEEN ADDED AS NEW PRODUCT");
    return 1;
}

int updateProduct(string const &shopName, string pdName = "") {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2";
    string tempLine, confirm;
    map<int, Product> prodList;

    if (pdName.empty()) {
        GET_PRODUCT_NAME:
        Ask("Enter the product name (or C to cancel): ");
        getline(cin, pdName);
        Clear();
        if (isCancel(pdName)) return 2;
        //VERIFY PRODUCT EXISTENCE
        if (isUniqueProduct(shopName, pdName)) {
            DisplayInfo("NO PRODUCT HAS THIS NAME");
            goto GET_PRODUCT_NAME;
        }
    }

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream hstFile(shopName + "_hist.txt", ios::app);
    vector<string> parts;
    unsigned int pos = 1;

    if (!oldList || !updatedProductList || !hstFile) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }

    while (getline(oldList, tempLine)) {
        parts = splitStr(tempLine, ":::");
        if (pdName == parts[0]) {
            Clear();

            Product tmpPd;
            initializeProductFromProdLine(tempLine, &tmpPd);
            DisplayMessage("========== UPDATE PROCEDURE ========");
            DisplayMessage("======= CURRENT PRODUCT STATE ======");
            printProduct(tmpPd, to_string(pos));
            DisplayMessage("====================================");

            Product nextPd;
            int getPdState = getProductFromInput(nextPd, &tmpPd, "GIVE UPDATED INFORMATION OF YOUR PRODUCT", shopName);

            if (getPdState == 2) {
                updatedProductList << tempLine << endl;
            } else {
                updatedProductList << nextPd.name << ":::" << nextPd.description << ":::" << nextPd.price << ":::"
                                   << nextPd.qtInStock << ":::" << nextPd.alertQt << endl;
                hstFile << getDateString() << " " << tmpPd.name << " has been renamed " << nextPd.name << endl;
            }

        } else {
            updatedProductList << tempLine << endl;
        }
        pos++;
    }

    updatedProductList.close();
    oldList.close();
    hstFile.close();
    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());
    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        DisplayInfo("PRODUCT HAS BEEN UPDATED");
        return 1;
    }
    ErrorHandler("SOME ERRORS OCCURS WHILE UPDATING PRODUCT! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 0;
}

int updateProduct(string const &shopName, unsigned int pos = 0) {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2";
    string tempLine, confirm, posStr;
    map<int, Product> prodList;
    unsigned int currentLine = 1;

    if (pos <= 0) {
        GET_PRODUCT_NUMBER:
        Ask("Enter the product number (or C to cancel): ");
        getline(cin, posStr);
        Clear();
        getAllProducts(shopName, prodList);
        pos = (int) prodList.size();
        if (isCancel(posStr)) return 2;
        if (!isValidIntChoice(posStr, pos, 1)) {
            DisplayInfo("NO PRODUCT HAS THIS NUMBER");
            goto GET_PRODUCT_NUMBER;
        }
        pos = stoi(posStr);
    }

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream hstFile(shopName + "_hist.txt", ios::app);

    if (!oldList || !updatedProductList || !hstFile) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }

    while (getline(oldList, tempLine)) {
        if (currentLine == pos) {
            Clear();
            Product tmpPd;
            initializeProductFromProdLine(tempLine, &tmpPd);
            DisplayMessage("========== UPDATE PROCEDURE ========");
            DisplayMessage("======= CURRENT PRODUCT STATE ======");
            printProduct(tmpPd, to_string(pos));
            Product nextPd;
            int getPdState = getProductFromInput(nextPd, &tmpPd, "GIVE UPDATED INFORMATION OF YOUR PRODUCT", shopName);

            if (getPdState == 2) {
                updatedProductList << tempLine << endl;
            } else {
                updatedProductList << nextPd.name << ":::" << nextPd.description << ":::" << nextPd.price << ":::"
                                   << nextPd.qtInStock << ":::" << nextPd.alertQt << endl;
                hstFile << getDateString() << " " << tmpPd.name << " has been renamed " << nextPd.name << endl;
            }

        } else {
            updatedProductList << tempLine << endl;
        }

        currentLine++;
    }

    updatedProductList.close();
    oldList.close();
    hstFile.close();
    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());
    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        DisplayInfo("PRODUCT HAS BEEN UPDATED");
        return 1;
    }
    ErrorHandler("SOME ERRORS OCCURS WHILE UPDATING PRODUCT! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 0;
}

int removeProduct(string const &shopName, unsigned int pos = 0) {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2";
    string tempLine, confirm, posStr;
    map<int, Product> prodList;
    unsigned int currentLine = 1;
    if (pos <= 0) {
        GET_PRODUCT_NUMBER:
        Ask("Enter the product number (or C to cancel): ");
        getline(cin, posStr);
        Clear();
        getAllProducts(shopName, prodList);
        pos = (int) prodList.size();
        if (isCancel(posStr)) return 2;
        if (!isValidIntChoice(posStr, pos, 1)) {
            DisplayInfo("NO PRODUCT HAS THIS NUMBER");
            goto GET_PRODUCT_NUMBER;
        }
        pos = stoi(posStr);
    }

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream hstFile(shopName + "_hist.txt", ios::app);

    if (!oldList || !updatedProductList || !hstFile) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }
    while (getline(oldList, tempLine)) {
        if (currentLine == pos) {
            currentLine++;
            Ask("Do you really want to delete '" + splitStr(tempLine, ":::")[0] +
                "' ? enter 1 to confirm or anything else to cancel: ");
            getline(cin, confirm);
            Clear();
            if (!isValidIntChoice(confirm, 1, 1)) {
                updatedProductList.close();
                oldList.close();
                hstFile.close();
                remove(updatedListName.c_str());
                DisplayInfo("DELETION HAS BEEN CANCELED");
                return 2;
            }
            Product tmp;
            initializeProductFromProdLine(tempLine, &tmp);
            hstFile << getDateString() << tmp.name << " has been deleted from products in shop " << endl;
            continue;
        }
        updatedProductList << tempLine << endl;
    }
    updatedProductList.close();
    oldList.close();
    hstFile.close();
    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());
    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        DisplayInfo("PRODUCT HAS BEEN DELETED");
        return 1;
    }
    ErrorHandler("SOME ERRORS OCCURS WHILE UPDATING PRODUCT! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 0;
}

int removeProduct(string const &shopName, string name = "") {
    string const oldListName = shopName + "_stocks.txt", updatedListName = shopName + "_stocks.txt.upd2";
    string tempLine, confirm, pdName;
    vector<string> parts;
    map<int, Product> prodList;
    map<int, Product>::iterator itr;

    if (name.empty()) {
        unsigned short name_found = 0;
        GET_PRODUCT_NAME:
        Ask("Enter the product name (or C to cancel): ");
        getline(cin, pdName);
        Clear();
        getAllProducts(shopName, prodList);
        if (isCancel(pdName)) return 2;
        for (itr = prodList.begin(); itr != prodList.end(); itr++) {
            if (itr->second.name == pdName) name_found = 1;
        }
        if (!name_found) goto GET_PRODUCT_NAME;
        name = pdName;
    }

    ofstream updatedProductList(updatedListName);
    ifstream oldList(oldListName);
    ofstream hstFile(shopName + "_hist.txt", ios::app);

    if (!oldList || !updatedProductList || !hstFile) {
        ErrorHandler("COULD NOT ACCESS OR CREATE REQUIRED FILES");
        return 0;
    }

    while (getline(oldList, tempLine)) {
        parts = splitStr(tempLine, ":::");
        if (parts[0] == name) {
            Clear();
            Ask("Do you really want to delete '" + parts[0] +
                "' ? enter 1 to confirm or anything else to cancel: ");
            getline(cin, confirm);
            Clear();
            if (!isValidIntChoice(confirm, 1, 1)) {
                updatedProductList.close();
                oldList.close();
                hstFile.close();
                remove(updatedListName.c_str());
                DisplayInfo("DELETION HAS BEEN CANCELED");
                return 2;
            }
            hstFile << getDateString() << name << " has been deleted from products in shop " << endl;
            continue;
        }
        updatedProductList << tempLine << endl;
    }
    updatedProductList.close();
    oldList.close();
    hstFile.close();
    int const removeOldList = remove(oldListName.c_str());
    int const renameUpdatedList = rename(updatedListName.c_str(), oldListName.c_str());
    if (removeOldList == 0 && renameUpdatedList == 0) {
        Clear();
        DisplayInfo("PRODUCT HAS BEEN DELETED");
        return 1;
    }
    ErrorHandler("SOME ERRORS OCCURS WHILE DELETING PRODUCT! FATAL DATA CORRUPTION MIGHT OCCUR");
    return 0;
}
