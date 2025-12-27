#include "DiceCalculator/Operators/AttackRoll.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Operators/Advantage.h"
#include <stdexcept>
#include <cmath>
#include <functional>

namespace DiceCalculator::Operators
{
	bool AttackRoll::Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (operands.size() != 2)
		{
			return false;
		}

		// For attack rolls, ensure that in the first operand there is exactly one d20 dice roll
		auto const& firstOperand = operands[0];
		bool hasD20First = false;
		if(ValidateAttackRollOperand(firstOperand, hasD20First))
		{
			return hasD20First;
		}
		return false;
	}

	int AttackRoll::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("AttackRoll operands are invalid.");
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

		for (const auto& record : records1)
		{
			if (record.Sides != 20)
			{
				continue;				
			}

			if (record.RolledValue >= CriticalHitThreshold)
			{
				return 1; // Critical hit
			}

			if (record.RolledValue <= CriticalMissThreshold)
			{
				return 0; // Critical miss
			}
		}

		return result1 >= result2 ? 1 : 0;
	}

	bool AttackRoll::ValidateAttackRollOperand(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& operand, bool& hasD20) const
	{
		DiceCounterVisitor visitor;
		operand->Accept(visitor);
		if (visitor.D20Count == 1)
		{
			hasD20 = true;
			return true;
		}
		else if (visitor.D20Count == 0)
		{
			hasD20 = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	Distribution AttackRoll::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(operands))
		{
			throw std::runtime_error("AttackRoll operands are invalid.");
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

		for (const auto& [value1, prob1, d20Lhs] : d1.GetData())
		{
			if (d20Lhs == 0)
			{
				throw std::runtime_error("First operand must include a d20 roll for attack roll evaluation.");
			}

			for (const auto& [value2, prob2, d20Rhs] : d2.GetData())
			{
				bool comparisonResult = value1 >= value2;
				if (d20Lhs >= CriticalHitThreshold)
				{
					comparisonResult = true; // Critical hit
				}
				else if(d20Lhs <= CriticalMissThreshold)
				{
					comparisonResult = false; // Critical miss
				}
				
				int outcomeValue = comparisonResult ? 1 : 0;
				double outcomeProb = prob1 * prob2;
				result.AddOutcome(outcomeValue, outcomeProb, d20Lhs);
			}
		}

		return result;
	}

	bool AttackRoll::IsEqual(const DiceOperator& other) const
	{
		return dynamic_cast<const AttackRoll*>(&other) != nullptr;
	}

	std::vector<OperatorRegistry::Entry> AttackRoll::Register()
	{
		registered = true;
		return {
			{ OperatorRegistry::Entry{"AttackRoll", OperatorRegistry::Arity::Function, []() { return std::make_shared<AttackRoll>(); }} }
		};
	}
}