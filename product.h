//
// Created by arnaudskovich on 09/07/22.
//

#ifndef INF200_PRODUCT_H
#define INF200_PRODUCT_H

#include "global.h"

struct Product {
    string name;
    string description;
    unsigned int price;
    unsigned int qtInStock;
    unsigned int alertQt;
};

void initializeProduct(string const &tmpName, string const &tmpDescription, unsigned int const &tmpPrice,
                       unsigned int const &tmpQt, unsigned int const &alertQt, Product *pd);

typedef struct Product Product;

void initializeProductFromProdLine(string const &prodLine, Product *pd);

void initializeProduct(string const &tmpName, string const &tmpDescription, unsigned int const &tmpPrice,
                       unsigned int const &tmpQt, unsigned int const &alertQt, Product *pd);

//TODO implement Product <vector> logic;
int productManager(string const &shopName);

int showAllProducts(string const &shopName);

void getAllProducts(string const &shopName, map<int, Product> &pdList);

int getProduct(string const &shopName, unsigned int pos, Product *pd);

int getProduct(string const &shopName, string pdName, Product *pd);

int searchProduct(string const &shopName, string word);

int searchProduct(string const &shopName, map<int, Product> &container, string word);

int addProduct(string const &shopName);

int updateProduct(string const &shopName, string pdName);

int updateProduct(string const &shopName, unsigned int pos);

int removeProduct(string const &shopName, unsigned int pos);

int removeProduct(string const &shopName, string name);

int isValidProductName(string const &productName);

void printProduct(Product const &pd, string const &pos);

#endif //INF200_PRODUCT_H
