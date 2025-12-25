#include "DiceCalculator/Operators/Subtraction.h"

namespace DiceCalculator::Operators
{
	bool Subtraction::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		// Require at least one operand (unary minus is supported as subtracting nothing)
		return operands.size() >= 1;
	}

	int Subtraction::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.empty())
			return 0;

		// Evaluate first operand
		operands[0]->Accept(visitor);
		int total = visitor.GetResult();

		// Subtract subsequent operands
		for (size_t i = 1; i < operands.size(); ++i)
		{
			operands[i]->Accept(visitor);
			total -= visitor.GetResult();
		}

		return total;
	}

	Distribution Subtraction::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.empty())
		{
			return Distribution();
		}

		// Start with the distribution of the first operand
		operands[0]->Accept(visitor);
		Distribution totalDistribution = visitor.GetDistribution();

		// For every subsequent operand, convolve by subtracting its distribution
		for (size_t i = 1; i < operands.size(); ++i)
		{
			operands[i]->Accept(visitor);
			const Distribution& opDistribution = visitor.GetDistribution();

			Distribution newTotalDistribution;
			// Combine the current total distribution with the new operand distribution (subtraction)
			for (const auto& [totalValue, totalProb] : totalDistribution)
			{
				for (const auto& [opValue, opProb] : opDistribution)
				{
					int newValue = totalValue - opValue;
					double newProb = totalProb * opProb;
					newTotalDistribution[newValue] += newProb;
				}
			}
			totalDistribution = std::move(newTotalDistribution);
		}

		return totalDistribution;
	}
}