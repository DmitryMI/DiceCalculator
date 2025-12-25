#pragma once

#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/IRandom.h"

namespace DiceCalculator::Evaluation
{
	class RollAstVisitor : public Evaluation::DiceAstVisitor
	{
	public:
		RollAstVisitor(IRandom& random) : m_Random(random), m_Result(0) {}

		void Visit(const DiceCalculator::Expressions::ConstantNode& node) override;
		void Visit(const DiceCalculator::Expressions::DiceNode& node) override;
		void Visit(const DiceCalculator::Expressions::OperatorNode& node) override;

		int GetResult() const { return m_Result; }

	private:
		int m_Result;
		IRandom& m_Random;
	};
}