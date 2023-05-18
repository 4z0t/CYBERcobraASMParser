#pragma once
#include "Definitions.h"
#include <memory>
#include <string>
#include <vector>

namespace riscv
{
	using std::string;
	using std::make_unique;
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

	class JumpTransformer :public virtual InstructionTransformer
	{
	public:
		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}

	};

	class LoadImmediateTransformer :public virtual InstructionTransformer
	{
	public:
		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}

	};

	class LoadExternalTransformer :public virtual InstructionTransformer
	{
	public:
		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}

	};

	class DirectTransformer : public virtual InstructionTransformer
	{
	public:

		DirectTransformer(ALUOP op)
			:m_op(op)
		{
		}

		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}
	protected:
		ALUOP m_op;
	};

	class BranchTransformer : public DirectTransformer
	{
	public:

		BranchTransformer(ALUOP op)
			:DirectTransformer(op)
		{
		}

		virtual CYBERCobraInstruction Transform(Instruction const& instr)
		{
		}
	};


	using InstructionTransformer_ptr = unique_ptr<InstructionTransformer>;

	class InstructionParser
	{
	public:
		InstructionParser() {};
		InstructionParser(Operation op, string const& s, InstructionTransformer_ptr&& transformer)
			: m_op(op), m_s(s), m_transformer(std::move(transformer))
		{
		};
		~InstructionParser() {};

	private:
		Operation m_op;
		string m_s;
		InstructionTransformer_ptr m_transformer;
	};


	const InstructionParser parsers[]
	{
		{Operation::ADD, "add", make_unique<DirectTransformer>(ALUOP::ALU_ADD)},
		{Operation::SUB, "sub", make_unique<DirectTransformer>(ALUOP::ALU_SUB)},
		{Operation::XOR, "xor", make_unique<DirectTransformer>(ALUOP::ALU_XOR)},
		{Operation::OR,  "or",  make_unique<DirectTransformer>(ALUOP::ALU_OR)},
		{Operation::AND, "and", make_unique<DirectTransformer>(ALUOP::ALU_AND)},
		{Operation::SLL, "sll", make_unique<DirectTransformer>(ALUOP::ALU_SLL)},
		{Operation::SRL, "srl", make_unique<DirectTransformer>(ALUOP::ALU_SRL)},
		{Operation::SRA, "sra", make_unique<DirectTransformer>(ALUOP::ALU_SRA)},
		{Operation::SLT, "slt", make_unique<DirectTransformer>(ALUOP::ALU_SLTS)},
		{Operation::SLTU,"sltu",make_unique<DirectTransformer>(ALUOP::ALU_SLTU)},
		{Operation::LI,  "li",  make_unique<LoadImmediateTransformer>()},
		{Operation::J,   "j",   make_unique<JumpTransformer>()},
		{Operation::BEQ, "beq", make_unique<BranchTransformer>(ALUOP::ALU_EQ)},
		{Operation::BNE, "bne", make_unique<BranchTransformer>(ALUOP::ALU_NE)},
		{Operation::BLT, "blt", make_unique<BranchTransformer>(ALUOP::ALU_LTS)},
		{Operation::BGE, "bge", make_unique<BranchTransformer>(ALUOP::ALU_GES)},
		{Operation::BLTU,"bltu",make_unique<BranchTransformer>(ALUOP::ALU_LTU)},
		{Operation::BGEU,"bgeu",make_unique<BranchTransformer>(ALUOP::ALU_GEU)},
	};


}