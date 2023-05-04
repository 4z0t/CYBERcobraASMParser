#pragma once
#include "Definitions.h"
#include <memory>
#include <string>
#include <vector>

namespace riscv
{
	using std::string;
	using std::vector;
	using std::unique_ptr;
	using namespace CYBERCobra;


	enum class Operation
	{
		ADD,
		SUB,
		XOR,
		OR,
		AND,
		SLL,
		SRL,
		SRA,
		SLT,
		SLTU,
		LI,
		J,
		BEQ,
		BNE,
		BLT,
		BGE,
		BLTU,
		BGEU,
	};

	struct Instruction
	{
		Operation op;


	};




	class InstructionTransformer
	{
	public:
		InstructionTransformer() {};

		virtual CYBERCobraInstruction Transform(Instruction const&) = 0;

		virtual	~InstructionTransformer() {};

	private:
	};

	class DirectTransformer : public InstructionTransformer
	{
	public:

		DirectTransformer(ALUOP op)
			:m_op(op)
		{
		}

		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}
	private:
		ALUOP m_op;
	};



	using InstructionTransformer_ptr = unique_ptr<InstructionTransformer>;

	class InstructionParser
	{
	public:
		InstructionParser() {};
		InstructionParser(Operation op, string const& s, InstructionTransformer_ptr transformer)
			: m_op(op), m_s(s), m_transformer(std::move(transformer))
		{
		};
		~InstructionParser() {};

	private:
		Operation m_op;
		string m_s;
		InstructionTransformer_ptr m_transformer;
	};


	const vector<InstructionParser> parsers
	{
		{Operation::ADD, "add", std::make_unique<DirectTransformer>(ALUOP::ALU_ADD)},
		{Operation::SUB, "sub", std::make_unique<DirectTransformer>(ALUOP::ALU_SUB)},
		{Operation::XOR, "xor", std::make_unique<DirectTransformer>(ALUOP::ALU_XOR)},
		{Operation::OR,  "or",  std::make_unique<DirectTransformer>(ALUOP::ALU_OR)},
		{Operation::AND, "and", std::make_unique<DirectTransformer>(ALUOP::ALU_AND)},
		{Operation::SLL, "sll", std::make_unique<DirectTransformer>(ALUOP::ALU_SLL)},
		{Operation::SRL, "srl", std::make_unique<DirectTransformer>(ALUOP::ALU_SRL)},
		{Operation::SRA, "sra", std::make_unique<DirectTransformer>(ALUOP::ALU_SRA)},
		{Operation::SLT, "slt", std::make_unique<DirectTransformer>(ALUOP::ALU_SLTS)},
		{Operation::SLTU,"sltu",std::make_unique<DirectTransformer>(ALUOP::ALU_SLTU)},
		{Operation::LI,  "li",  nullptr},
		{Operation::J,   "j",   nullptr},
		{Operation::BEQ, "beq", std::make_unique<DirectTransformer>(ALUOP::ALU_EQ)},
		{Operation::BNE, "bne", std::make_unique<DirectTransformer>(ALUOP::ALU_NE)},
		{Operation::BLT, "blt", std::make_unique<DirectTransformer>(ALUOP::ALU_LTS)},
		{Operation::BGE, "bge", std::make_unique<DirectTransformer>(ALUOP::ALU_GES)},
		{Operation::BLTU,"bltu",std::make_unique<DirectTransformer>(ALUOP::ALU_LTU)},
		{Operation::BGEU,"bgeu",std::make_unique<DirectTransformer>(ALUOP::ALU_GEU)},
	};


}