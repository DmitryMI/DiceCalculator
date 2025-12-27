#pragma once

#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include "DiceCalculator/Distribution.h"

namespace DiceCalculator::Evaluation
{
	class CombinationAstVisitor : public Evaluation::DiceAstVisitor
	{
	public:
		void Visit(const DiceCalculator::Expressions::ConstantNode& node) override;
		void Visit(const DiceCalculator::Expressions::DiceNode& node) override;
		void Visit(const DiceCalculator::Expressions::OperatorNode& node) override;

		const DiceCalculator::Distribution& GetDistribution() const { return m_Distribution; }

	private:
		DiceCalculator::Distribution m_Distribution;
	};
}