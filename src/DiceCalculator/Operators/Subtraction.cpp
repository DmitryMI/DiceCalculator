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

	Distribution Subtraction::Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		throw std::runtime_error("Combination evaluation not implemented.");
	}

	std::vector<OperatorRegistry::Entry> Subtraction::Register()
	{
		registered = true;
		return {
			{ OperatorRegistry::Entry{"-", OperatorRegistry::Arity::Binary, []() { return std::make_shared<Subtraction>(); }} }
		};
	}
}