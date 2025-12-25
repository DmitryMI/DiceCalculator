#include "DiceCalculator/Operators/Addition.h"

namespace DiceCalculator::Operators
{
	bool Addition::Validate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return true;
	}

	int Addition::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		int total = 0;
		for (auto& op : operands)
		{
			op->Accept(visitor);
			total += visitor.GetResult();
		}
		return total;
	}

	Distribution Addition::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		Distribution totalDistribution = { {0, 1} };
		for (auto& op : operands)
		{
			op->Accept(visitor);
			const Distribution& opDistribution = visitor.GetDistribution();
			Distribution newTotalDistribution;
			// Combine the current total distribution with the new operand distribution
			for (const auto& [totalValue, totalProb] : totalDistribution)
			{
				for (const auto& [opValue, opProb] : opDistribution)
				{
					int newValue = totalValue + opValue;
					double newProb = totalProb * opProb;
					newTotalDistribution[newValue] += newProb;
				}
			}
			totalDistribution = std::move(newTotalDistribution);
		}
		return totalDistribution;
	}
}