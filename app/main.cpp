/*
 * main.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/parser/Browser.h>
#include <sequence/DisplayUtils.h>

#include <algorithm>
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

void printUsage(const char* prgName) {
    printf("USAGE: %s [-R] [PATH]\n", prgName);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    try {
        if (argc > 3 || argc == 1)
            printUsage(argv[0]);

        bool recursive = false;
        if (argc == 3)
            if (string(argv[1]) == "-R")
                recursive = true;
            else
                printUsage(argv[0]);

        typedef vector<sequence::BrowseItem> Items;
        const Items items = sequence::parser::browse(argv[argc == 3 ? 2 : 1]);
        copy(items.begin(), items.end(), ostream_iterator<sequence::BrowseItem>(cout, "\n"));
        return EXIT_SUCCESS;
    } catch (exception& e) {
        cerr << "Unexpected error : " << e.what() << endl;
    }
    return EXIT_FAILURE;
}
