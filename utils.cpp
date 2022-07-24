//
// Created by arnaudskovich on 09/07/22.
//
#include "global.h"


void ErrorHandler(string const &message) {
    DisplayError(message);
    DisplayMessage("PROGRAM WILL EXIT");
    exit(EXIT_FAILURE);
}

void QUIT() {
    exit(EXIT_SUCCESS);
}

void Clear() {
    system("clear");
}

void Ask(string const &message) {
    cout << endl << message;
}

void DisplayMessage(string const &message) {
    cout << endl << message << endl;
}

void DisplayError(string const &error) {
    DisplayMessage("X X X X " + error + " X X X X");
}

void DisplayInfo(string const &info) {
    DisplayMessage("! ! ! ! " + info + " ! ! ! !");
}

int isPrevMenu(string &input) {
    return (input == "0");
}

int isValidIntChoice(string const &input, unsigned int const &max, unsigned int const &min) {
    regex authorized("[0-9]+");
    string max_;
    max_ += to_string(max);
    unsigned short l_ = max_.length();
    unsigned short l = input.length();
    if (regex_match(input, authorized) && l <= l_) {
        int const entered = stoi(input);
        return (min <= entered && entered <= max);
    }
    return 0;
}

void PrintListAndAsk(vector<string> const &list, short const &show_prev = 0) {
    unsigned int const &length = list.size();
    string possible_values, finalAsk;
    Blank();
    if (length == 0 && show_prev) {
        Ask("Enter 0 for previous menu: ");
        return;
    }
    for (int k = 1; k <= length; k++) {
        cout << k << "-" << list[k - 1] << endl;
    }
    if (length > 0) {
        finalAsk += "Enter ";
        possible_values += "1";
    }
    if (length > 1) {
        finalAsk += "a digit included in ";
        possible_values += " - " + to_string(length);
    }
    if (show_prev) possible_values += (" (or 0 for previous menu)");
    finalAsk += possible_values + ": ";
    Ask(finalAsk);
}

void PrintList(vector<string> const &list) {
    unsigned int const &length = list.size();
    Blank();
    for (int k = 1; k <= length; k++) {
        cout << k << "-" << list[k - 1] << endl;
    }
    Blank();
}

void Blank() {
    cout << endl;
}

vector<string> splitStr(string const &str, string const &delim) {
    vector<string> parts;
    string currentPart;
    int const delimLength = (int) delim.size();
    int currentPosToSearchFrom = 0;
    int currentDelimPos = (int) str.find(delim);
    if(delimLength > 0){
        while (currentDelimPos != string::npos) {
            currentPart = str.substr(currentPosToSearchFrom, currentDelimPos - currentPosToSearchFrom);
            parts.emplace_back(currentPart);
            currentPosToSearchFrom = currentDelimPos + delimLength;
            currentDelimPos = (int) str.find(delim, currentPosToSearchFrom);
        }
        parts.emplace_back(str.substr(currentPosToSearchFrom));
    }else{
        for(auto &letter : str){
            string tmp;
            tmp.push_back(letter);
            parts.emplace_back(tmp);
        }
    }
    return parts;
}

/**
 * Return current date formatted as 31/12/2022 13:50:53
 * @return
 */
string getDateString(){
    time_t time_v;
    tm *time_s;
    char container[60];

    time(&time_v);
    time_s = localtime(&time_v);

    strftime(container, 60, "%d/%m/%Y %T", time_s);

    return container;
}

string strToLower(string const &str){
    string next;
    char tmp;
    vector<string> letters = splitStr(str,"");
    for(auto & letter : letters){
        tmp = (char)tolower((int) letter[0]);
        next.push_back(tmp);
    }
    return next;
}