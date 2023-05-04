#pragma once
#include "Definitions.h"
#include <memory>
#include <string>

namespace riscv
{
	using std::string;
	using std::unique_ptr;
	using namespace CYBERCobra;


	enum class Instruction
	{

	};




	class InstructionTransformer
	{
	public:
		InstructionTransformer() {};
		virtual	~InstructionTransformer() {};

	private:

	};

	using InstructionTransformer_ptr = unique_ptr<InstructionTransformer>;

	class InstructionParser
	{
	public:
		InstructionParser() {};
		InstructionParser(Instruction instr, string const& s, InstructionTransformer_ptr transformer)
			: m_instr(instr), m_s(s), m_transformer(std::move(transformer))
		{

		};
		~InstructionParser() {};

	private:
		Instruction m_instr;
		string m_s;
		InstructionTransformer_ptr m_transformer;
	};


}