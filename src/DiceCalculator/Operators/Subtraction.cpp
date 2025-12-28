#include "DiceCalculator/Operators/Subtraction.h"

namespace DiceCalculator::Operators
{
	bool Subtraction::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		// Require at least one operand (unary minus is supported as subtracting nothing)
		return operands.size() >= 1;
	}

	int Subtraction::Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.empty())
		{
			return 0;
		}

		std::vector<DiceCalculator::Evaluation::RollAstVisitor::DiceRollRecord> totalRecords;

		operands[0]->Accept(visitor);
		int total = visitor.GetResult();
		const auto& records = visitor.GetDiceRecords();
		totalRecords.insert(totalRecords.end(), records.begin(), records.end());

		for (size_t i = 1; i < operands.size(); ++i)
		{
			operands[i]->Accept(visitor);
			total -= visitor.GetResult();
			const auto& records = visitor.GetDiceRecords();
			totalRecords.insert(totalRecords.end(), records.begin(), records.end());
		}
		visitor.SetDiceRecords(totalRecords);

		return total;
	}

	Distribution Subtraction::Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
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

	bool Subtraction::IsEqual(const DiceOperator& other) const
	{
		return dynamic_cast<const Subtraction*>(&other) != nullptr;
	}

	std::vector<Combination> Subtraction::Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.empty())
		{
			return {};
		}

		// Start with combinations of the first operand
		operands[0]->Accept(visitor);
		std::vector<Combination> total = visitor.GetCombinations();

		// Subtract subsequent operands' totals, concatenating their rolls
		for (size_t i = 1; i < operands.size(); ++i)
		{
			operands[i]->Accept(visitor);
			const auto& opCombinations = visitor.GetCombinations();

			std::vector<Combination> newTotal;
			newTotal.reserve(static_cast<size_t>(total.size()) * static_cast<size_t>(opCombinations.size()));

			for (const auto& t : total)
			{
				for (const auto& oc : opCombinations)
				{
					Combination combined;
					combined.TotalValue = t.TotalValue - oc.TotalValue;
					combined.Rolls = t.Rolls;
					combined.Rolls.insert(combined.Rolls.end(), oc.Rolls.begin(), oc.Rolls.end());
					newTotal.push_back(std::move(combined));
				}
			}

			total = std::move(newTotal);
		}

		return total;
	}

	std::vector<RegistryEntry> Subtraction::Register()
	{
		return {
			{ RegistryEntry{"-", Arity::Binary, []() { return std::make_shared<Subtraction>(); }} }
		};
	}
}