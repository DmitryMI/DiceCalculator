#pragma once

#include "DiceCalculator/Operators/DiceOperator.h"
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"

namespace DiceCalculator::Operators
{
	class Comparison : public DiceOperator
	{
	public:
		enum class Mode
		{
			LessThan,
			LessThanOrEqual,
			Equal,
			NotEqual,
			GreaterThanOrEqual,
			GreaterThan
		};

		Comparison(Mode mode) : m_Mode(mode){}

		bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		int Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		Distribution Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		bool IsEqual(const DiceOperator& other) const override;
		Mode GetMode() const { return m_Mode; }
	private:
		Mode m_Mode;
	};
}