#pragma once

#include "DiceCalculator/Parsing/IParser.h"
#include "DiceCalculator/Operators/IRegistry.h"

namespace DiceCalculator::Parsing
{
	class BoostSpiritParser : public IParser
	{
	public:
		BoostSpiritParser(std::shared_ptr<Operators::IRegistry> registry);

		virtual ~BoostSpiritParser() = default;
		std::shared_ptr<DiceCalculator::Expressions::DiceAst> Parse(const std::string& input) const override;
		std::string Reconstruct(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast) const override;
	private:
		std::string ReconstructInternal(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast, bool isChildOfOperator) const;
		std::shared_ptr<Operators::IRegistry> m_Registry;
	};
}