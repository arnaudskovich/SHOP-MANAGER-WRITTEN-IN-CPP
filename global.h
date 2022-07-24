//
// Created by arnaudskovich on 09/07/22.
//

#ifndef INF200_GLOBAL_H
#define INF200_GLOBAL_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <regex>
#include <cctype>
#include <ctime>

using namespace std;

void ErrorHandler(string const &message);

void QUIT();

void DisplayMessage(string const &message);

void DisplayError(string const &error);

void DisplayInfo(string const &info);

void Ask(string const &message);

void Clear();

void Blank();

void PrintListAndAsk(vector<string> const &list, short const &show_prev);

void PrintList(vector<string> const &list);

string getDateString();

int isPrevMenu(string &input);

int isValidIntChoice(string const &input, unsigned int const &max, unsigned int const &min);

vector <string> splitStr(string const &str, string const &delim);

string strToLower(string const &str);

#endif //INF200_GLOBAL_H
