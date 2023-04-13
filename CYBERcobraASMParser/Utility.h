#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <exception>

using namespace std;


vector< string> Split(const string& s, const string& separator)
{
	vector< string> result;

	size_t pos = s.find(separator);
	size_t initialPos = 0;

	while (pos != std::string::npos) {
		result.push_back(s.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = s.find(separator, initialPos);
	}
	result.push_back(s.substr(initialPos, std::min(pos, s.size()) - initialPos + 1));

	return result;
}

