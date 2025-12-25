#pragma once

namespace DiceCalculator
{
	namespace Evaluation
	{
		class DiceAstVisitor;
	}
}

namespace DiceCalculator::Expressions
{
	class DiceAst
	{
	public:
		virtual ~DiceAst() = default;

		virtual void Accept(DiceCalculator::Evaluation::DiceAstVisitor& visitor) const = 0;
	};
}