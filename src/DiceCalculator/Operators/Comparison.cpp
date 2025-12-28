#include "DiceCalculator/Operators/Comparison.h"
#include <stdexcept>

namespace DiceCalculator::Operators
{
	bool Comparison::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return operands.size() == 2;
	}

	int Comparison::Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("Comparison operator requires exactly one operand.");
		}

		operands[0]->Accept(visitor);
		int result1 = visitor.GetResult();
		auto records1 = visitor.GetDiceRecords();
		operands[1]->Accept(visitor);
		int result2 = visitor.GetResult();
		auto records2 = visitor.GetDiceRecords();

		auto recordsTotal = records1;
		recordsTotal.insert(recordsTotal.end(), records2.begin(), records2.end());
		visitor.SetDiceRecords(recordsTotal);

		switch (m_Mode)
		{
			case Mode::LessThan:
				return result1 < result2 ? 1 : 0;
			case Mode::LessThanOrEqual:
				return result1 <= result2 ? 1 : 0;
			case Mode::Equal:
				return result1 == result2 ? 1 : 0;
			case Mode::NotEqual:
				return result1 != result2 ? 1 : 0;
			case Mode::GreaterThanOrEqual:
				return result1 >= result2 ? 1 : 0;
			case Mode::GreaterThan:
				return result1 > result2 ? 1 : 0;
			default:
				throw std::runtime_error("Invalid comparison mode.");
		}
		
	}


	Distribution Comparison::Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("Comparison operands are invalid.");
		}

		operands[0]->Accept(visitor);
		Distribution d1 = visitor.GetDistribution();

		if (d1.Size() == 0)
		{
			throw std::runtime_error("First operand has an empty distribution.");
		}

		operands[1]->Accept(visitor);
		Distribution d2 = visitor.GetDistribution();

		if (d2.Size() == 0)
		{
			throw std::runtime_error("First operand has an empty distribution.");
		}

		Distribution result;

		for (const auto& [value1, prob1] : d1.GetData())
		{
			for (const auto& [value2, prob2] : d2.GetData())
			{
				bool comparisonResult = false;
				switch (m_Mode)
				{
					case Mode::LessThan:
						comparisonResult = (value1 < value2);
						break;
					case Mode::LessThanOrEqual:
						comparisonResult = (value1 <= value2);
						break;
					case Mode::Equal:
						comparisonResult = (value1 == value2);
						break;
					case Mode::NotEqual:
						comparisonResult = (value1 != value2);
						break;
					case Mode::GreaterThanOrEqual:
						comparisonResult = (value1 >= value2);
						break;
					case Mode::GreaterThan:
						comparisonResult = (value1 > value2);
						break;
					default:
						throw std::runtime_error("Invalid comparison mode.");
				}
				int outcomeValue = comparisonResult ? 1 : 0;
				double outcomeProb = prob1 * prob2;
				result.AddOutcome(outcomeValue, outcomeProb);
			}
		}

		return result;
	}


	std::vector<Combination> Comparison::Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("Comparison operands are invalid.");
		}

		// Evaluate combinations for both operands
		operands[0]->Accept(visitor);
		auto leftCombinations = visitor.GetCombinations();

		operands[1]->Accept(visitor);
		auto rightCombinations = visitor.GetCombinations();

		if (leftCombinations.empty() || rightCombinations.empty())
		{
			// If any side has no combinations, result is empty set
			return {};
		}

		std::vector<Combination> result;
		result.reserve(static_cast<size_t>(leftCombinations.size()) * static_cast<size_t>(rightCombinations.size()));

		for (const auto& lc : leftCombinations)
		{
			for (const auto& rc : rightCombinations)
			{
				bool comparisonResult = false;
				switch (m_Mode)
				{
					case Mode::LessThan:
						comparisonResult = (lc.TotalValue < rc.TotalValue);
						break;
					case Mode::LessThanOrEqual:
						comparisonResult = (lc.TotalValue <= rc.TotalValue);
						break;
					case Mode::Equal:
						comparisonResult = (lc.TotalValue == rc.TotalValue);
						break;
					case Mode::NotEqual:
						comparisonResult = (lc.TotalValue != rc.TotalValue);
						break;
					case Mode::GreaterThanOrEqual:
						comparisonResult = (lc.TotalValue >= rc.TotalValue);
						break;
					case Mode::GreaterThan:
						comparisonResult = (lc.TotalValue > rc.TotalValue);
						break;
					default:
						throw std::runtime_error("Invalid comparison mode.");
				}

				Combination combined;
				combined.TotalValue = comparisonResult ? 1 : 0;
				combined.Rolls = lc.Rolls;
				combined.Rolls.insert(combined.Rolls.end(), rc.Rolls.begin(), rc.Rolls.end());

				result.push_back(std::move(combined));
			}
		}

		return result;
	}

	bool Comparison::IsEqual(const DiceOperator& other) const
	{
		if (const auto* otherComp = dynamic_cast<const Comparison*>(&other))
		{
			return m_Mode == otherComp->m_Mode;
		}
		return false;
	}
}