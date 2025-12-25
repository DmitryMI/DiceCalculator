#include "DiceCalculator/Operators/Advantage.h"
#include <stdexcept>

namespace DiceCalculator::Operators
{
	bool Advantage::Validate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		return operands.size() == 1;
	}

	int Advantage::Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		if (!Validate(visitor, operands))
		{
			throw std::runtime_error("Advantage operator requires exactly one operand.");
		}

		operands[0]->Accept(visitor);
		int result1 = visitor.GetResult();
		operands[0]->Accept(visitor);
		int result2 = visitor.GetResult();
		if (m_Mode == Mode::Advantage)
		{
			return std::max(result1, result2);
		}
		else
		{
			return std::min(result1, result2);
		}
	}

	Distribution Advantage::Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
	{
		// Note: Full implementation of distribution calculation for advantage/disadvantage is complex
		return Distribution();
	}
}