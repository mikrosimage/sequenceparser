/*
 * main.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/parser/Browser.h>
#include <sequence/DisplayUtils.h>

#include <boost/chrono/chrono.hpp>
#include <boost/chrono/chrono_io.hpp>

#include <algorithm>
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace boost::chrono;

void printUsage(const char* prgName) {
    printf("USAGE: %s [-R] [PATH]\n", prgName);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    try {
        if (argc > 3 || argc == 1)
            printUsage(argv[0]);

        bool recursive = false;
        if (argc == 3) {
            if (string(argv[1]) == "-R") {
                recursive = true;
            } else {
                printUsage(argv[0]);
            }
        }

        high_resolution_clock::time_point start = high_resolution_clock::now();

        typedef vector<sequence::BrowseItem> Items;
        const Items items = sequence::parser::browse(argv[argc == 3 ? 2 : 1], recursive);

        ostringstream stream;
        stream << "Listing " << items.size() << " items took " << duration_cast<milliseconds>(high_resolution_clock::now() - start) << endl;
        copy(items.begin(), items.end(), ostream_iterator<sequence::BrowseItem>(stream, "\n"));
        printf("%s\n", stream.str().c_str());

        return EXIT_SUCCESS;
    } catch (exception& e) {
        cerr << "Unexpected error : " << e.what() << endl;
    }
    return EXIT_FAILURE;
}
