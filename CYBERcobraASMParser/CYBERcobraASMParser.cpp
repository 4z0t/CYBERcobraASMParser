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

	NOP, RET,
	PUSH, POP,

	MOV,

	ADD, SUB,

	INC, DEC,

	IMUL, IDIV,

	AND, OR, XOR,
	NOT, NEG,

	SHL, SHR,
	SAL, SAR,

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

	bool IsAdress()
	{
		return adress != 0;
	}

	bool IsConstant()
	{
		return !IsAdress();
	}
};




struct ASMInstruction
{
	ASM_OP op = ASM_OP::NOP;
	int jump = -1;
	Argument a1{};
	Argument a2{};
};



unordered_map<string, ASM_OP> STRING_TO_ASM_OP
{
	{"nop",ASM_OP::NOP},
	{"ret",ASM_OP::RET},
	{"push", ASM_OP::PUSH},
	{"pop", ASM_OP::POP},
	{"mov", ASM_OP::MOV},

	{"add", ASM_OP::ADD},
	{"jmp", ASM_OP::JMP},
	{"jle", ASM_OP::JLE},
	{"cmp", ASM_OP::CMP},

	{"sub",ASM_OP::SUB},
	{"inc",ASM_OP::INC},
	{"dec",ASM_OP::DEC},
	{"imul",ASM_OP::IMUL},
	{"idiv",ASM_OP::IDIV},

	{"and",ASM_OP::AND},
	{"or",ASM_OP::OR,},
	{"xor",ASM_OP::XOR},
	{"not",ASM_OP::NOT},
	{"neg",ASM_OP::NEG},


	{"shl",ASM_OP::SHL},
	{"sal",ASM_OP::SAL},
	{"shr",ASM_OP::SHR},
	{"sar",ASM_OP::SAR},

	{"je",ASM_OP::JE},
	{"jne",ASM_OP::JNE},
	{"jz",ASM_OP::JZ  },
	{"jg",ASM_OP::JG},
	{"jge",ASM_OP::JGE},
	{"jl",ASM_OP::JL},
	{"jle",ASM_OP::JLE},
};




ALUOP ASMopToALUOP(ASM_OP op)
{
	switch (op)
	{
		//ignored
	case ASM_OP::NOP:
	case ASM_OP::RET:
	case ASM_OP::PUSH:
	case ASM_OP::POP:
	case ASM_OP::MOV:
	case ASM_OP::CMP:
	case ASM_OP::JMP:
	case ASM_OP::LABEL:
		break;


		//not implemented
	case ASM_OP::INC:
	case ASM_OP::DEC:
	case ASM_OP::IMUL:
	case ASM_OP::IDIV:
	case ASM_OP::NEG:
	case ASM_OP::NOT:
	case ASM_OP::JZ:
		break;


	case ASM_OP::ADD:
		return ALUOP::ALU_ADD;
	case ASM_OP::SUB:
		return ALUOP::ALU_SUB;
	case ASM_OP::AND:
		return ALUOP::ALU_AND;
	case ASM_OP::OR:
		return ALUOP::ALU_OR;
	case ASM_OP::XOR:
		return ALUOP::ALU_XOR;
	case ASM_OP::SHL:
		return ALUOP::ALU_SLL;
	case ASM_OP::SHR:
		return ALUOP::ALU_SRL;
	case ASM_OP::SAL:
		return ALUOP::ALU_SLL;
	case ASM_OP::SAR:
		return ALUOP::ALU_SRA;
	case ASM_OP::JE:
		return ALUOP::ALU_EQ;
	case ASM_OP::JNE:
		return ALUOP::ALU_NE;

		//todo fix
	case ASM_OP::JG:
		return ALUOP::ALU_GES;
	case ASM_OP::JGE:
		return ALUOP::ALU_GES;
	case ASM_OP::JL:
		return ALUOP::ALU_LTS;
	case ASM_OP::JLE:
		return ALUOP::ALU_LTS;
	default:
		break;
	}
	return ALUOP::ALU_ADD;
}





struct Labels
{
	uint index = 0;
	unordered_map<string, int> labels;
};



ASMInstruction ParseInstruction(ASM_OP op, const vector<string>& line, Labels& labels)
{
	ASMInstruction instr{};
	instr.op = op;
	string lbl;
	switch (op)
	{
		//ignore these
	case ASM_OP::NOP:
	case ASM_OP::RET:
	case ASM_OP::PUSH:
	case ASM_OP::POP:
	case ASM_OP::INC:
	case ASM_OP::DEC:
		break;

		//two argumnets
	case ASM_OP::MOV:
	case ASM_OP::ADD:
	case ASM_OP::SUB:
	case ASM_OP::IMUL:
	case ASM_OP::IDIV:
	case ASM_OP::AND:
	case ASM_OP::OR:
	case ASM_OP::XOR:
	case ASM_OP::CMP:
		instr.a1 = Argument(line[1]);
		instr.a2 = Argument(line[2]);
		break;

		//these are tricky (can have  1 or 2 args)
	case ASM_OP::SHR:
	case ASM_OP::SHL:
	case ASM_OP::SAR:
	case ASM_OP::SAL:
		instr.a1 = Argument(line[1]);
		if (line.size() == 3)
			instr.a2 = Argument(line[2]);
		else
			instr.a2.constant = 1;
		break;

		//one arg
	case ASM_OP::NOT:
	case ASM_OP::NEG:
		instr.a1 = Argument(line[1]);
		break;
		//labels
	case ASM_OP::JE:
	case ASM_OP::JNE:
	case ASM_OP::JZ:
	case ASM_OP::JG:
	case ASM_OP::JGE:
	case ASM_OP::JL:
	case ASM_OP::JLE:
	case ASM_OP::JMP:
		lbl = line[1];
		if (labels.labels.find(lbl) == labels.labels.end())
		{
			labels.labels[lbl] = labels.index;
			instr.jump = labels.index;
			labels.index++;
		}
		else
		{
			instr.jump = labels.labels[lbl];
		}
		break;
	default:
		break;
	}
	return instr;
}



vector<ASMInstruction> ToASMInstructions(const vector<vector< string>>& splitted_lines)
{

	Labels labels;

	vector<ASMInstruction> asm_instructions(splitted_lines.size());
	transform(
		splitted_lines.begin(),
		splitted_lines.end(),
		asm_instructions.begin(),
		[&](const vector<string>& line)
		{
			ASMInstruction instr;

	auto token = line[0];
	//check for label
	if (regex_match(token, LABEL_REGEX))
	{
		auto lbl = token.substr(0, token.size() - 1);
		instr.op = ASM_OP::LABEL;
		if (labels.labels.find(lbl) == labels.labels.end())
		{
			instr.jump = labels.index;
			labels.labels[lbl] = labels.index;
			labels.index++;
		}
		else
		{
			instr.jump = labels.labels[lbl];
		}
		return instr;
	}


	instr = ParseInstruction(STRING_TO_ASM_OP.at(token), line, labels);

	return instr;
		});



	return asm_instructions;
}



vector<CYBERCobraInstruction> ASMToCobra(const vector<ASMInstruction>& asm_instructions)
{
	vector<CYBERCobraInstruction> cobra_instructions;
	cobra_instructions.reserve(asm_instructions.size());

	unordered_map<int, int> labels;

	ASMInstruction prev;

	for (size_t i = 0; i < asm_instructions.size(); i++)
	{
		auto cur = asm_instructions[i];

		switch (cur.op)
		{
		case ASM_OP::LABEL:
		{
			labels[cur.jump] = cobra_instructions.size();
		}
		break;

		case ASM_OP::JMP:
		{
			cobra_instructions.push_back(CYBERCobra::Jump(cur.jump));
		};
		break;
		case ASM_OP::JE:
		case ASM_OP::JNE:
		case ASM_OP::JZ:
		case ASM_OP::JG:
		case ASM_OP::JGE:
		case ASM_OP::JL:
		case ASM_OP::JLE:
		{
			if (prev.op != ASM_OP::CMP)
			{
				throw logic_error("previous must be a CMP op");
			}

			if (prev.a2.IsConstant())
			{
				cobra_instructions.push_back(CYBERCobra::PushConstantAt(prev.a2.constant, CONST_ADRESS));
				cobra_instructions.push_back(CYBERCobra::ConditionalJump(ASMopToALUOP(cur.op), prev.a1.adress, CONST_ADRESS, cur.jump));
			}
			else
			{
				cobra_instructions.push_back(CYBERCobra::ConditionalJump(ASMopToALUOP(cur.op), prev.a1.adress, prev.a2.adress, cur.jump));
			}


		};
		break;


		case ASM_OP::MOV:
		{
			if (cur.a2.IsConstant())
			{
				cobra_instructions.push_back(CYBERCobra::PushConstantAt(cur.a2.constant, cur.a1.adress));
			}
			else
			{
				cobra_instructions.push_back(CYBERCobra::ALUInstruction(ALUOP::ALU_ADD, cur.a2.adress, 0, cur.a1.adress));
			}
		};
		break;


		case ASM_OP::ADD:
		case ASM_OP::SUB:
		case ASM_OP::IMUL:
		case ASM_OP::IDIV:
		case ASM_OP::AND:
		case ASM_OP::OR:
		case ASM_OP::XOR:
		case ASM_OP::SHR:
		case ASM_OP::SHL:
		case ASM_OP::SAR:
		case ASM_OP::SAL:
		{
			if (cur.a2.IsConstant())
			{
				cobra_instructions.push_back(CYBERCobra::PushConstantAt(cur.a2.constant, CONST_ADRESS));
				cobra_instructions.push_back(CYBERCobra::ALUInstruction(ASMopToALUOP(cur.op), cur.a1.adress, CONST_ADRESS, cur.a1.adress));
			}
			else
			{
				cobra_instructions.push_back(CYBERCobra::ALUInstruction(ASMopToALUOP(cur.op), cur.a1.adress, cur.a2.adress, cur.a1.adress));
			}

		};
		break;

		default:
			break;
		}

		prev = cur;
	}


	/*{
		for (const auto& label : labels)
		{
			cout << label.first << ": " << label.second << endl;
		}
	}*/

	// after we put in jumps proper offsets


	for (size_t i = 0; i < cobra_instructions.size(); i++)
	{
		auto& instr = cobra_instructions[i];
		if (instr.b || instr.j)
		{
			instr.offset = labels[instr.offset] - i;
		}
	}


	for (size_t i = 0; i < cobra_instructions.size(); i++)
	{
		auto& instr = cobra_instructions[i];
		cout << i << "\t" << CYBERCobra::ToString(instr) << endl;
	}




	return cobra_instructions;
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

	vector<ASMInstruction>  asm_instructions = ToASMInstructions(splitted_lines);

	{// remove redundant ops
		vector<ASMInstruction>  new_asm_instructions;

		copy_if(
			asm_instructions.begin(),
			asm_instructions.end(),
			back_inserter(new_asm_instructions),
			[](const ASMInstruction& instr)
			{
				switch (instr.op)
				{
				case ASM_OP::NOP:
				case ASM_OP::RET:
				case ASM_OP::PUSH:
				case ASM_OP::POP:
				case ASM_OP::INC:
				case ASM_OP::DEC:
					return false;
				}
		return true;
			});
		asm_instructions = move(new_asm_instructions);
	}
	/*{
		for (const auto& instr : asm_instructions)
		{
			Argument a1 = instr.a1;
			Argument a2 = instr.a2;
			cout << static_cast<uint>(instr.op) << "\t" << instr.jump << "\t"
				<< ((a1.adress == 0) ? "const  " : "adress ") << ((a1.adress == 0) ? a1.constant : a1.adress) << "\t"
				<< ((a2.adress == 0) ? "const  " : "adress ") << ((a2.adress == 0) ? a2.constant : a2.adress) << "\t"
				<< endl;
		}
	}*/

	vector<CYBERCobraInstruction> result = ASMToCobra(asm_instructions);


	return result;
}

int main(int argc, char** argv)
{
	string path = "lab.txt";

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

