#pragma once

#include "DiceCalculator/Operators/DiceOperator.h"
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Evaluation/ConvolutionAstVisitor.h"
#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"
#include "DiceCalculator/Operators/IRegistry.h"

namespace DiceCalculator::Operators
{
	class Advantage : public DiceOperator
	{
	public:
		enum class Mode
		{
			Advantage,
			Disadvantage
		};

		Advantage(Mode mode) : m_Mode(mode) {}

		bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		int Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		Distribution Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		std::vector<Combination> Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;

		Mode GetMode() const { return m_Mode; }

		bool IsEqual(const DiceOperator& other) const override;

		static std::vector<RegistryEntry> Register();

	private:
		Mode m_Mode;

		int GetRerolls(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const;
		
	};
}