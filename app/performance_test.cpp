/*
 * main.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/parser/ParserUtils.h>
#include <sequence/DisplayUtils.h>

#include <boost/chrono/chrono.hpp>
#include <boost/chrono/chrono_io.hpp>

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace sequence;
using namespace sequence::parser::details;
using namespace boost::chrono;
using namespace boost::filesystem;

void printUsage(const char* prgName) {
    printf("USAGE: %s [-R] [PATH]\n", prgName);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    try {
        vector<path> paths;
        SequencePattern p1("file-1.",".cr2",7);
        SequencePattern p2("file-.",".jpeg",6);

        for (size_t i = 0; i < 150000; ++i)
            paths.push_back(string("/root/") + sequence::instanciatePattern(p1, i));
        for (size_t i = 0; i < 200000; ++i)
            paths.push_back(string("/root/two/") + sequence::instanciatePattern(p2, i));

        high_resolution_clock::time_point start = high_resolution_clock::now();

        typedef vector<sequence::BrowseItem> Items;
        Parser parser;

        for_each(paths.begin(), paths.end(), parser.functor());

        Items items = parser.getResults();
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
