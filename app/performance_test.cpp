/*
 * main.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/parser/ParserUtils.h>
#include <sequence/DisplayUtils.h>

#include <boost/bind.hpp>
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
        sequence::SequencePattern pattern;
        pattern.prefix = "file-1.";
        pattern.suffix = ".cr2";
        pattern.padding = 7;

        for (size_t i = 0; i < 150000; ++i)
            paths.push_back(string("/root/") + sequence::instanciatePattern(pattern, i));

        high_resolution_clock::time_point start = high_resolution_clock::now();

        typedef vector<sequence::BrowseItem> Items;
        Parser parser;

        for_each(paths.begin(), paths.end(), boost::bind(&Parser::operator(), boost::ref(parser), _1));

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
