#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <exception>
#include <memory>
#include <regex>
#include <assert.h>

#include "Utility.h"
#include "Definitions.h"

using namespace std;


regex RBP_REGEX("\\[rbp-([\\d]*)\\]");

uint GetRegisterIndex(const string& rbp_s, uint start = 0)
{
	smatch rbp_match;

	if (!regex_search(rbp_s, rbp_match, RBP_REGEX))
		throw exception();

	return stoi(rbp_match[1]) / 4;
}


vector<string> ReadLines(const string& path)
{

	ifstream asm_file;
	asm_file.open(path);

	if (!asm_file.is_open())
	{
		throw exception();
	}

	list<string> lines;

	do
	{
		string line;
		getline(asm_file, line);
		lines.push_back(line);
	} while (!asm_file.eof());


	return vector<string>(lines.begin(), lines.end());
}



vector<Instruction> ProcessLines(const vector<string>& lines)
{
	vector<vector<string>> splitted_lines(lines.size());

	transform(lines.begin(), lines.end(), splitted_lines.begin(),
		[](const string& s) {
			return Split(s, " ");
		});

	transform(splitted_lines.begin(), splitted_lines.end(), splitted_lines.begin(),
		[](const vector<string>& splitted_line)
		{
			vector<string> new_splitted_line;
			copy_if(splitted_line.begin(),
				splitted_line.end(),
				back_inserter(new_splitted_line),
				[](const string& s) {
					return !s.empty() && s != "DWORD" && s != "PTR";
				});
			return new_splitted_line;

		});


	for (const auto& line : splitted_lines)
	{
		for (const auto& s : line)
		{
			cout << "'" << s << "' ";
		}
		cout << endl;
	}
	vector<Instruction> result;






	return result;
}

int main(int argc, char** argv)
{
	string path = "a.txt";

	vector <string> lines;
	vector<Instruction> instructions;
	try
	{
		lines = ReadLines(path);
		for (auto& line : lines)
		{
			cout << line << endl;
		}

		instructions = ProcessLines(lines);

	}
	catch (exception& e)
	{
		cerr << "Unable to read file";
		return 1;
	}




	return 0;
}

