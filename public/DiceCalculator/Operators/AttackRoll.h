#pragma once

#include "DiceCalculator/Operators/DiceOperator.h"
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Evaluation/ConvolutionAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"

namespace DiceCalculator::Operators
{
	class AttackRoll : public DiceOperator, AutoRegister<AttackRoll>
	{
	public:

		AttackRoll() {}

		bool Validate(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		int Evaluate(DiceCalculator::Evaluation::RollAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		Distribution Evaluate(DiceCalculator::Evaluation::ConvolutionAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		std::vector<Combination> Evaluate(DiceCalculator::Evaluation::CombinationAstVisitor& visitor, std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const override;
		bool IsEqual(const DiceOperator& other) const override;

		static std::vector<OperatorRegistry::Entry> Register();

	private:
		constexpr static int CriticalHitThreshold = 20;
		constexpr static int CriticalMissThreshold = 1;

		class DiceCounterVisitor : public DiceCalculator::Evaluation::DiceAstVisitor
		{
		public:
			int D20Count = 0;

			void Visit(const DiceCalculator::Expressions::ConstantNode& node) override
			{

			}

			void Visit(const DiceCalculator::Expressions::DiceNode& node) override
			{
				if (node.GetSides() == 20)
				{
					D20Count += node.GetRolls();
				}
			}

			void Visit(const DiceCalculator::Expressions::OperatorNode& node) override
			{
				for (const auto& operand : node.GetOperands())
				{
					operand->Accept(*this);
				}
			}
		};

		bool ValidateAttackRollOperand(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& operand, bool& hasD20) const;
	};
}