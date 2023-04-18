#pragma once
#include <string>
#include <bitset>
#include <sstream>

typedef unsigned int uint;


enum class REGISTER_INDEX :uint
{
	UNKNOWN = 0,
	EAX,
	EBX,
	ECX,
	EDX,
	RBP,
	RSP,
	CNST,
};

const uint CONST_ADRESS = static_cast<uint>(REGISTER_INDEX::CNST);



enum class ALUOP : uint
{
	ALU_ADD = 0b00000,
	ALU_SUB = 0b01000,

	ALU_XOR = 0b00100,
	ALU_OR = 0b00110,
	ALU_AND = 0b00111,

	ALU_SRA = 0b01101,
	ALU_SRL = 0b00101,
	ALU_SLL = 0b00001,

	ALU_LTS = 0b11100,
	ALU_LTU = 0b11110,
	ALU_GES = 0b11101,
	ALU_GEU = 0b11111,
	ALU_EQ = 0b11000,
	ALU_NE = 0b11001,

	ALU_SLTS = 0b00010,
	ALU_SLTU = 0b00011,

};

std::string ALUOPToString(ALUOP op)
{
	switch (op)
	{
	case ALUOP::ALU_ADD:
		return "+";
	case ALUOP::ALU_SUB:
		return "-";
	case ALUOP::ALU_XOR:
		return "^";
	case ALUOP::ALU_OR:
		return "|";
	case ALUOP::ALU_AND:
		return "&";
	case ALUOP::ALU_SRA:
		return ">>>";
	case ALUOP::ALU_SRL:
		return ">>";
	case ALUOP::ALU_SLL:
		return "<<";
	case ALUOP::ALU_LTS:
		return "<s";
	case ALUOP::ALU_LTU:
		return "<u";
	case ALUOP::ALU_GES:
		return ">=s";
	case ALUOP::ALU_GEU:
		return ">=u";
	case ALUOP::ALU_EQ:
		return "==";
	case ALUOP::ALU_NE:
		return "!=";
	case ALUOP::ALU_SLTS:
		return "= <s";
	case ALUOP::ALU_SLTU:
		return "= <u";
	default:
		break;
	}
}


struct CYBERCobraInstruction
{
	uint write_adress : 5;
	union {
		int rf_const : 23;
		struct
		{
			int offset : 8;
			uint ra1 : 5;
			uint ra2 : 5;
			ALUOP op : 5;
		};
	};
	uint ws : 2;
	bool b;
	bool j;
};


namespace CYBERCobra
{

	CYBERCobraInstruction PushConstantAt(int constant, uint adress = CONST_ADRESS)
	{
		CYBERCobraInstruction instr{};
		instr.j = false;
		instr.b = false;
		instr.ws = 0b00;
		instr.rf_const = constant;
		instr.write_adress = adress;
		return instr;
	}


	CYBERCobraInstruction Jump(int offset)
	{
		CYBERCobraInstruction instr{};
		instr.j = true;
		instr.b = false;
		instr.offset = offset;
		return instr;
	}

	CYBERCobraInstruction ConditionalJump(ALUOP op, uint ra1, uint ra2, int offset)
	{
		CYBERCobraInstruction instr{};
		instr.j = false;
		instr.b = true;
		instr.ra1 = ra1;
		instr.ra2 = ra2;
		instr.offset = offset;
		instr.op = op;
		return instr;
	}

	CYBERCobraInstruction ReadTo(uint adress)
	{
		CYBERCobraInstruction instr{};
		instr.j = false;
		instr.b = false;
		instr.ws = 0b10;
		instr.write_adress = adress;
		return instr;
	}

	CYBERCobraInstruction ALUInstruction(ALUOP op, uint ra1, uint ra2, uint wa)
	{
		CYBERCobraInstruction instr{};
		instr.j = false;
		instr.b = false;
		instr.ws = 0b01;
		instr.ra1 = ra1;
		instr.ra2 = ra2;
		instr.write_adress = wa;
		instr.op = op;
		return instr;
	}

	bitset<32> ToBits(CYBERCobraInstruction instr)
	{
		using namespace std;
		bitset<32> repr{ 0 };
		repr |= (instr.j ? 1u : 0u) << 31;
		repr |= (instr.b ? 1u : 0u) << 30;
		repr |= instr.ws << 28;
		if (instr.ws == 0b00 && !(instr.b || instr.j))
		{
			repr |= instr.rf_const << 5;
		}
		else
		{
			repr |= static_cast<uint>(instr.op) << 23;
			repr |= instr.ra1 << 18;
			repr |= instr.ra2 << 13;
			repr |= instr.offset << 5;
		}
		repr |= instr.write_adress;
		return repr;
	}

	string ToBinary(CYBERCobraInstruction instr)
	{
		return ToBits(instr).to_string();
	}

	string ToHex(CYBERCobraInstruction instr)
	{
		using namespace std;
		stringstream ss;
		ss.width(8);
		ss.fill('0');
		ss << hex << ToBits(instr).to_ulong();
		return ss.str();
	}

	string Represent(CYBERCobraInstruction instr, int current_line = 0)
	{
		stringstream ss;


		const auto SQuare = [&](uint n) {
			ss << '[';
			ss.width(2);
			ss.fill(' ');
			ss << n << ']';
			return ' ';
		};


		if (instr.b)
		{
			ss << "J " << (instr.offset + current_line) << " if " << SQuare(instr.ra1);
			if (instr.ra2 || instr.op != ALUOP::ALU_ADD)
				ss << ALUOPToString(instr.op) << " " << SQuare(instr.ra2);
		}
		else if (instr.j)
		{
			ss << "J " << (instr.offset + current_line);
		}
		else if (instr.ws == 0b01)
		{
			ss << "[" << instr.write_adress << "]" << " <- " << SQuare(instr.ra1);
			if (instr.ra2 || instr.op != ALUOP::ALU_ADD)
				ss << ALUOPToString(instr.op) << " " << SQuare(instr.ra2);
		}
		else if (instr.ws == 0b00)
		{
			ss << "[" << instr.write_adress << "]" << " <- " << instr.rf_const;
		}
		else if (instr.ws == 0b10)
		{
			ss << "[" << instr.write_adress << "]" << " <- sw_i";
		}
		return ss.str();
	}

	string ToString(CYBERCobraInstruction instr, bool display_const = false)
	{
		using namespace std;
		stringstream ss;
		ss << bitset<1>(instr.j) << " "
			<< bitset<1>(instr.b) << " "
			<< bitset<2>(instr.ws) << " ";
		if (instr.ws == 0b00 && !(instr.b || instr.j))
		{
			ss << bitset<23>(instr.rf_const);
			if (display_const)
				ss << "[" << instr.rf_const << "]";
			ss << " ";
		}
		else
		{
			ss << bitset<5>(static_cast<uint>(instr.op)) << " "
				<< bitset<5>(instr.ra1) << " "
				<< bitset<5>(instr.ra2) << " "
				<< bitset<8>(instr.offset) << " ";
		}
		ss << bitset<5>(instr.write_adress);
		return ss.str();
	}

}