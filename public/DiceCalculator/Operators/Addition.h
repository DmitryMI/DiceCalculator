#pragma once

#include "DiceCalculator/Operators/DiceOperator.h"
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"

namespace DiceCalculator::Operators
{
	class Addition : public DiceOperator, public AutoRegister<Addition>
	{
	public:
		bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		int Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		Distribution Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		bool IsEqual(const DiceOperator& other) const override;
		static std::vector<OperatorRegistry::Entry> Register();
	};
}