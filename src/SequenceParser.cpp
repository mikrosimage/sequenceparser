//============================================================================
// Name        : SequenceParser.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "sequence/Item.h"

#include <boost/function.hpp>
#include <iostream>

using namespace std;
using namespace sequence;

namespace sequence {

namespace parser {

using boost::filesystem::path;
typedef vector<path> paths;

typedef boost::function<paths(const path&)> BrowseFolder;
typedef boost::function<void(const paths&, paths&)> SelectDirectory;

} // namespace parser

} // namespace sequence

parser::paths myFunction(const parser::path &p) {
	cout << " called !!!" << endl;
	return parser::paths();
}

int main() {
	parser::BrowseFolder f = myFunction;
	f(boost::filesystem::path("/s/t"));
	return 0;
}
