#pragma once

#include <vector>
#include <memory>
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/Combination.h"

namespace DiceCalculator::Evaluation
{
	class RollAstVisitor;
	class ConvolutionAstVisitor;
	class CombinationAstVisitor;
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

		virtual bool IsEqual(const DiceOperator& other) const = 0;
		virtual bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;

		virtual int Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
		virtual Distribution Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
		virtual std::vector<Combination> Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const = 0;
	};
}