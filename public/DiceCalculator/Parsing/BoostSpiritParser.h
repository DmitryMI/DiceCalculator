#pragma once

#include "DiceCalculator/Parsing/IParser.h"

namespace DiceCalculator::Parsing
{
	class BoostSpiritParser : public IParser
	{
	public:
		virtual ~BoostSpiritParser() = default;
		std::shared_ptr<DiceCalculator::Expressions::DiceAst> Parse(const std::string& input) const override;
		std::string Reconstruct(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast) const override;
	};
}