#pragma once

#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include <stdexcept>

namespace DiceCalculator::Evaluation
{
	void CombinationAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		m_Combinations = { Combination{ node.GetValue(), {} } };
	}

	void CombinationAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		const int rolls = node.GetRolls();
		const int sides = node.GetSides();

		// Start with a single empty outcome to build upon
		std::vector<Combination> outcomes;
		outcomes.push_back(Combination{ 0, {} });

		for (int r = 0; r < rolls; ++r)
		{
			std::vector<Combination> next;
			next.reserve(static_cast<size_t>(outcomes.size()) * static_cast<size_t>(sides));

			for (const auto& o : outcomes)
			{
				for (int face = 1; face <= sides; ++face)
				{
					Combination expanded;
					expanded.TotalValue = o.TotalValue + face;
					expanded.Rolls = o.Rolls;
					expanded.Rolls.push_back(Combination::Roll{ sides, face });
					next.push_back(std::move(expanded));
				}
			}

			outcomes = std::move(next);
		}

		m_Combinations = std::move(outcomes);
	}

	void CombinationAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		m_Combinations = node.GetOperator()->Evaluate(*this, node.GetOperands());
	}
}