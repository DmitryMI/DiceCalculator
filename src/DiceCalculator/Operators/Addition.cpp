#include "DiceCalculator/Operators/Addition.h"

namespace DiceCalculator::Operators
{
	bool Addition::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return true;
	}

	int Addition::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		int total = 0;
		std::vector<DiceCalculator::Evaluation::RollAstVisitor::DiceRollRecord> totalRecords;
		for (auto& op : operands)
		{
			op->Accept(visitor);
			total += visitor.GetResult();
			totalRecords.insert(totalRecords.end(), visitor.GetDiceRecords().begin(), visitor.GetDiceRecords().end());
		}
		visitor.SetDiceRecords(totalRecords);
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
			for (const auto& [totalValue, totalProb, d20Lhs] : totalDistribution)
			{
				for (const auto& [opValue, opProb, d20Rhs] : opDistribution)
				{
					int newValue = totalValue + opValue;
					double newProb = totalProb * opProb;
					newTotalDistribution[newValue].Probability += newProb;
					newTotalDistribution[newValue].D20 = d20Lhs;
				}
			}
			totalDistribution = std::move(newTotalDistribution);
		}
		return totalDistribution;
	}

	bool Addition::IsEqual(const DiceOperator& other) const
	{
		return dynamic_cast<const Addition*>(&other) != nullptr;
	}

	std::vector<OperatorRegistry::Entry> Addition::Register()
	{
		registered = true;
		return {
			{ OperatorRegistry::Entry{"+", OperatorRegistry::Arity::Binary, []() { return std::make_shared<Addition>(); }} }
		};
	}
}