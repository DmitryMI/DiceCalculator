#pragma once

#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/Combination.h"

namespace DiceCalculator::Evaluation
{
	class CombinationAstVisitor : public Evaluation::DiceAstVisitor
	{
	public:

		constexpr static int MaxCombinationsThreshold = 10000000;

		void Visit(const DiceCalculator::Expressions::ConstantNode& node) override;
		void Visit(const DiceCalculator::Expressions::DiceNode& node) override;
		void Visit(const DiceCalculator::Expressions::OperatorNode& node) override;

		// const DiceCalculator::Distribution& GetDistribution() const { return m_Distribution; }
		const std::vector<Combination>& GetCombinations() const { return m_Combinations; }

	private:
		std::vector<Combination> m_Combinations;
	};
}