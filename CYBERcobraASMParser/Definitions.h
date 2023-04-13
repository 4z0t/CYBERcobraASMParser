#pragma once


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
			char offset;
			uint ra1 : 5;
			uint ra2 : 5;
			ALUOP op : 5;
		};
	};
	uint ws : 2;
	bool b;
	bool j;
};
