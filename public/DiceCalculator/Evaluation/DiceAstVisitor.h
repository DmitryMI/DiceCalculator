#pragma once

namespace DiceCalculator
{
	namespace Expressions
	{
		class ConstantNode;
		class DiceNode;
		class OperatorNode;
	}
}

namespace DiceCalculator::Evaluation
{
	class DiceAstVisitor
	{
	public:
		virtual ~DiceAstVisitor() = default;

		virtual void Visit(const DiceCalculator::Expressions::ConstantNode& node) = 0;
		virtual void Visit(const DiceCalculator::Expressions::DiceNode& node) = 0;
		virtual void Visit(const DiceCalculator::Expressions::OperatorNode& node) = 0;
	};
}