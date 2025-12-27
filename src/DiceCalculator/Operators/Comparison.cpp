#include "DiceCalculator/Operators/Comparison.h"
#include <stdexcept>

namespace DiceCalculator::Operators
{
	bool Comparison::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return operands.size() == 2;
	}

	int Comparison::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
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


	Distribution Comparison::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
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

	bool Comparison::IsEqual(const DiceOperator& other) const
	{
		if (const auto* otherComp = dynamic_cast<const Comparison*>(&other))
		{
			return m_Mode == otherComp->m_Mode;
		}
		return false;
	}
}