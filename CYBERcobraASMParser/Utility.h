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

vector<vector< string>> FormatLines(const vector<string>& lines)
{
	vector<vector<string>> splitted_lines(lines.size());

	//split by spaces
	transform(lines.begin(), lines.end(), splitted_lines.begin(),
		[](const string& s) {
			return Split(s, " ");
		});

	//removing redundant strings
	transform(splitted_lines.begin(), splitted_lines.end(), splitted_lines.begin(),
		[](const vector<string>& splitted_line)
		{
			vector<string> new_splitted_line;

	//remove empty strings and DWORD PTR
	copy_if(splitted_line.begin(),
		splitted_line.end(),
		back_inserter(new_splitted_line),
		[](const string& s)
		{
			return !s.empty() && s != "DWORD" && s != "PTR";
		});
	//remove commas
	transform(new_splitted_line.begin(),
		new_splitted_line.end(),
		new_splitted_line.begin(),
		[](const string& s) {
			if (s[s.size() - 1] == ',')
			return s.substr(0, s.size() - 1);
	return s;
		});
	return new_splitted_line;

		});
	return splitted_lines;
}