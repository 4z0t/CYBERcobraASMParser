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
#include <unordered_map>
#include <optional>

#include "Utility.h"
#include "Definitions.h"

#define REG_OFFSET 16

using namespace std;

class LabelNotFoundException : public logic_error
{
	using logic_error::logic_error;
};

class UnknownRegisterException : public logic_error
{
	using logic_error::logic_error;
};


const regex RBP_REGEX("\\[rbp-([\\d]*)\\]");
const regex LABEL_REGEX("\\.[a-zA-Z0-9]*:");

enum class ASM_OP
{

	NOP,
	PUSH, POP,

	MOV,

	ADD, SUB,

	INC, DEC,

	IMUL, IDIV,

	AND, OR, XOR,
	NOT, NEG,

	SHL, SHR,

	LABEL,
	JMP,
	JE, JNE, // == !=
	JZ,      //  
	JG, JGE, // > >=
	JL, JLE, // < <=
	CMP
};



int GetRBPIndex(const string& rbp_s, int start = REG_OFFSET)
{
	smatch rbp_match;

	if (!regex_search(rbp_s, rbp_match, RBP_REGEX))
		return -1;

	return stoi(rbp_match[1]) / 4 + start;
}




REGISTER_INDEX GetRegisterIndex(const string& arg)
{
	if (arg == "eax") return REGISTER_INDEX::EAX;
	if (arg == "ebx") return REGISTER_INDEX::EBX;
	if (arg == "ecx") return REGISTER_INDEX::ECX;
	if (arg == "edx") return REGISTER_INDEX::EDX;
	if (arg == "rbp") return REGISTER_INDEX::RBP;
	if (arg == "rsp") return REGISTER_INDEX::RSP;
	return REGISTER_INDEX::UNKNOWN;
}

struct Argument
{
	uint adress = 0;
	int constant = 0;


	Argument() = default;

	Argument(const string& arg)
	{
		this->constant = 0;

		int _adress = GetRBPIndex(arg);
		if (_adress != -1) // it is an adress
		{
			this->adress = _adress;
			return;
		}

		REGISTER_INDEX reg = GetRegisterIndex(arg);
		if (reg != REGISTER_INDEX::UNKNOWN) //it is a register
		{
			this->adress = static_cast<uint>(reg);
			return;
		}

		this->adress = 0;//illegal adress means constant;
		this->constant = stoi(arg);
	}
};




struct ASMInstruction
{
	ASM_OP op = ASM_OP::NOP;
	int jump = -1;
	Argument a1{};
	Argument a2{};
};



unordered_map<string, ASM_OP> TO_ASM_OP
{
	{"push",ASM_OP::PUSH },
	{"pop",ASM_OP::POP },
	{"mov", ASM_OP::MOV},
	{"add", ASM_OP::ADD},
	{"jmp", ASM_OP::JMP},
	{"jle", ASM_OP::JLE},
	{"cmp", ASM_OP::CMP},
};














CYBERCobraInstruction ParseJmp(const vector<string>& line, int index, const unordered_map<string, int>& labels)
{
	string label = line[1];

	if (labels.find(label) == labels.end())
		throw LabelNotFoundException(label);

	int offset = labels.at(label) - index;

	CYBERCobraInstruction instr{};
	instr.j = true;
	instr.b = false;
	instr.offset = offset;

	return instr;
}



CYBERCobraInstruction ParseMov(const vector<string>& line)
{
	string arg1_s = line[1];
	string arg2_s = line[2];

	int arg1 = GetRBPIndex(arg1_s);
	int arg2 = GetRBPIndex(arg2_s);

	if (arg1 == -1)arg1 = static_cast<int>(GetRegisterIndex(arg1_s));
	if (arg2 == -1)arg2 = static_cast<int>(GetRegisterIndex(arg2_s));

	CYBERCobraInstruction instr{};


	return instr;
}


optional<CYBERCobraInstruction> ParseInstruction(ASM_OP op, const vector<string>& line, uint index, const unordered_map<string, int>& labels)
{
	switch (op)
	{
	case ASM_OP::PUSH:
		break;
	case ASM_OP::POP:
		break;
	case ASM_OP::MOV:
		return ParseMov(line);
	case ASM_OP::ADD:
		break;
	case ASM_OP::SUB:
		break;
	case ASM_OP::INC:
		break;
	case ASM_OP::DEC:
		break;
	case ASM_OP::IMUL:
		break;
	case ASM_OP::IDIV:
		break;
	case ASM_OP::AND:
		break;
	case ASM_OP::OR:
		break;
	case ASM_OP::XOR:
		break;
	case ASM_OP::NOT:
		break;
	case ASM_OP::NEG:
		break;
	case ASM_OP::SHL:
		break;
	case ASM_OP::SHR:
		break;
	case ASM_OP::JMP:
		return ParseJmp(line, index, labels);
	case ASM_OP::JE:
		break;
	case ASM_OP::JNE:
		break;
	case ASM_OP::JZ:
		break;
	case ASM_OP::JG:
		break;
	case ASM_OP::JGE:
		break;
	case ASM_OP::JL:
		break;
	case ASM_OP::JLE:
		break;
	case ASM_OP::CMP:
		break;
	default:
		break;
	}
	return nullopt;
}



optional<CYBERCobraInstruction> LineToInstruction(const vector<string>& line, uint index, const unordered_map<string, int>& labels)
{
	if (line.size() == 1)
		return nullopt;

	optional<CYBERCobraInstruction> instruction;
	string op = line[0];

	if (TO_ASM_OP.find(op) == TO_ASM_OP.end())
		return nullopt;

	auto asm_op = TO_ASM_OP.at(op);

	instruction = ParseInstruction(asm_op, line, index, labels);

	return instruction;
}



vector<CYBERCobraInstruction> ProcessLines(const vector<string>& lines)
{

	auto splitted_lines = FormatLines(lines);


	{
		size_t i = 0;

		for (const auto& line : splitted_lines)
		{
			cout << i << "\t";
			for (const auto& s : line)
			{
				cout << "'" << s << "' ";
			}
			i++;
			cout << endl;
		}
	}





	vector<vector<string>> splitted_lines_nolabels;
	unordered_map<string, int> labels;
	size_t index = 0;
	copy_if(splitted_lines.begin(), splitted_lines.end(), back_inserter(splitted_lines_nolabels),
		[&](const vector<string>& splitted_line)
		{
			index++;
	if (splitted_line.size() != 1)
		return true;

	auto token = splitted_line[0];
	if (!regex_match(token, LABEL_REGEX))
		return true;

	labels[token.substr(0, token.size() - 1)] = index - labels.size() - 1;
	return false;
		});


	{
		size_t i = 0;

		for (const auto& line : splitted_lines_nolabels)
		{
			cout << i << "\t";
			for (const auto& s : line)
			{
				cout << "'" << s << "' ";
			}
			LineToInstruction(line, i, labels);
			i++;
			cout << endl;
		}
	}

	for (const auto& [k, v] : labels)
	{
		cout << k << " " << v << endl;
	}






	vector<CYBERCobraInstruction> result;






	return result;
}

int main(int argc, char** argv)
{
	string path = "a.txt";

	vector <string> lines;
	vector<CYBERCobraInstruction> instructions;
	try
	{
		lines = ReadLines(path);

		instructions = ProcessLines(lines);

	}
	catch (LabelNotFoundException& e)
	{
		cerr << "Unable to find label " << e.what();
		return 1;
	}





	return 0;
}

