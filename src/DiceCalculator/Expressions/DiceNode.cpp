#pragma once

#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
	void DiceNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

	bool DiceNode::IsEqual(const DiceAst& other) const
	{
		if (const auto* otherDice = dynamic_cast<const DiceNode*>(&other))
		{
			return m_Rolls == otherDice->m_Rolls && m_Sides == otherDice->m_Sides;
		}
		return false;
	}
}