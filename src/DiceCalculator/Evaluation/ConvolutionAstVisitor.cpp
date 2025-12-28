#pragma once

#include "DiceCalculator/Evaluation/ConvolutionAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include <stdexcept>

namespace DiceCalculator::Evaluation
{
	void ConvolutionAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		m_Distribution = { {node.GetValue(), 1.0} };
	}

	void ConvolutionAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		int rolls = node.GetRolls();
		int sides = node.GetSides();

		// No rolls => deterministic 0
		if (rolls <= 0)
		{
			m_Distribution = { {0, 1.0} };
			return;
		}

		// Single die distribution
		Distribution singleDie;
		if (sides <= 0)
		{
			throw std::runtime_error("DiceNode has non-positive sides");
		}
		else
		{
			double faceProb = 1.0 / static_cast<double>(sides);
			for (int face = 1; face <= sides; ++face)
			{
				singleDie.AddOutcome(face, faceProb);
			}
		}

		// Start with one die, then convolve `rolls` times
		m_Distribution = singleDie;
		for (int i = 1; i < rolls; ++i)
		{
			Distribution next;
			// Reserve approximate size: current distinct sums * sides
			next.Reserve(m_Distribution.Size() * (sides > 0 ? static_cast<size_t>(sides) : 1));

			for (auto const& pair : m_Distribution)
			{
				int baseValue = pair.first;
				double baseProb = pair.second;
				// convolve with single die outcomes
				for (auto const& facePair : singleDie)
				{
					int newValue = baseValue + facePair.first;
					double newProb = baseProb * facePair.second;
					next.AddOutcome(newValue, newProb);
				}
			}

			m_Distribution = std::move(next);
		}
	}

	void ConvolutionAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		m_Distribution = node.GetOperator()->Evaluate(*this, node.GetOperands());
	}
}