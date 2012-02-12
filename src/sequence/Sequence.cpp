#include "Sequence.h"
#include "DisplayUtils.h"

#include <algorithm>
#include <sstream>

using namespace std;

namespace sequence {

std::pair<boost::filesystem::path, SequencePattern> parsePattern( const std::string& absoluteFilename ) {
	const boost::filesystem::path normalized(absoluteFilename);
	const string filename = normalized.filename().string();
	typedef string::const_iterator CItr;
	const CItr paddingBegin = std::find(filename.begin(), filename.end(),
			gPaddingChar);
	if (paddingBegin == filename.end())
		throw runtime_error( string("Unable to find '#' in filename '") + filename + '\'' );
	const CItr paddingEnd = std::upper_bound(paddingBegin, filename.end(), gPaddingChar);
	return make_pair( //
			normalized.parent_path(), //
			SequencePattern(string(filename.begin(), paddingBegin), //
			string(paddingEnd, filename.end()), //
			distance(paddingBegin, paddingEnd)));
}

template<typename T, size_t count>
struct CharStack {
	CharStack(unsigned int value) : index(0) {
		for (; value; value /= 10)
			push("0123456789"[value % 10]);
	}
	void push(T c) { assert(index<count); buffer[index++] = c; }
	size_t size() const { return index; }
	bool empty() const { return index == 0; }
	const T top() const { return buffer[index - 1]; }
	void pop() { assert(index!=0); --index; }
private:
	T buffer[count];
	size_t index;
};

std::string instanciatePattern(const SequencePattern &pattern, unsigned int frame) {
	string result;
	result.reserve(100);
	result += pattern.prefix;
	const bool padding = pattern.padding > 1;
	CharStack<unsigned char, 32> stack(frame);
	if (padding) {
		if (stack.size() > pattern.padding) {
			ostringstream ss;
			ss << "Unable to put " << frame << " in pattern '" << pattern << '\"';
			throw runtime_error(ss.str());
		}
		while (stack.size() < pattern.padding)
			stack.push('0');
	}
	for (; !stack.empty(); stack.pop())
		result.push_back(stack.top());
	result += pattern.suffix;
	return result;
}

} // namespace sequence
