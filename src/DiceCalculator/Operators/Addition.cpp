#include "DiceCalculator/Operators/Addition.h"

namespace DiceCalculator::Operators
{
	bool Addition::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return true;
	}

	int Addition::Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
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

	Distribution Addition::Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
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

	std::vector<Combination> Addition::Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		// Start with a single empty combination
		std::vector<Combination> total = { Combination{ 0, {} } };

		for (auto& op : operands)
		{
			op->Accept(visitor);
			const auto& opCombinations = visitor.GetCombinations();

			std::vector<Combination> newTotal;
			newTotal.reserve(static_cast<size_t>(total.size()) * static_cast<size_t>(opCombinations.size()));

			for (const auto& t : total)
			{
				for (const auto& oc : opCombinations)
				{
					Combination combined;
					combined.TotalValue = t.TotalValue + oc.TotalValue;
					combined.Rolls = t.Rolls;
					combined.Rolls.insert(combined.Rolls.end(), oc.Rolls.begin(), oc.Rolls.end());
					newTotal.push_back(std::move(combined));
				}
			}

			total = std::move(newTotal);
		}

		return total;
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