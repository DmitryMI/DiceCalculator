#pragma once

#include <memory>
#include <string>

namespace DiceCalculator::Expressions
{
	class DiceAst;
}

namespace DiceCalculator::Parsing
{
	class IParser
	{
	public:
		virtual ~IParser() = default;
		virtual std::shared_ptr<DiceCalculator::Expressions::DiceAst> Parse(const std::string& input) const = 0;
		virtual std::string Reconstruct(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast) const = 0;
	};
}