#pragma once

#include <vector>
#include <memory>
#include "DiceCalculator/Distribution.h"

namespace DiceCalculator::Evaluation
{
	class RollAstVisitor;
	class DistributionAstVisitor;
}

namespace DiceCalculator::Expressions
{
	class DiceAst;
}

namespace DiceCalculator::Operators
{
	class DiceOperator
	{
	public:
		virtual ~DiceOperator() = default;

		virtual bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
		virtual int Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
		virtual Distribution Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
	};
}