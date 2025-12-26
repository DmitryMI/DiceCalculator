#pragma once

#include "DiceCalculator/Parsing/BoostSpiritParser.h"

namespace DiceCalculator::Parsing
{
	std::shared_ptr<DiceCalculator::Expressions::DiceAst> BoostSpiritParser::Parse(const std::string& input) const
	{
		return nullptr;
	}

	std::string BoostSpiritParser::Reconstruct(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast) const
	{
		return "";
	}
}