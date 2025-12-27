#pragma once

#include "DiceCalculator/Operators/DiceOperator.h"
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"

namespace DiceCalculator::Operators
{
	class Advantage : public DiceOperator, public AutoRegister<Advantage>
	{
	public:
		enum class Mode
		{
			Advantage,
			Disadvantage
		};

		Advantage(Mode mode, int rerolls = 2) : m_Mode(mode), m_Rerolls(rerolls) {}

		bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		int Roll(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		Distribution Evaluate(DiceCalculator::Evaluation::DistributionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		
		Mode GetMode() const { return m_Mode; }

		bool IsEqual(const DiceOperator& other) const override;

		static std::vector<std::pair<std::string_view, OperatorRegistry::Factory>> Register();

	private:
		Mode m_Mode;
		int m_Rerolls = 2;

		
	};
}