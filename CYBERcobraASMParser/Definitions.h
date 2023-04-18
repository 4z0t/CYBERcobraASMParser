#pragma once
#include <string>
#include <bitset>

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

};


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

	CYBERCobraInstruction PushConstantAt(int constant, uint adress)
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
		if (instr.ws == 0b00)
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
		bitset<32> repr;
		return "TODO";
	}

}