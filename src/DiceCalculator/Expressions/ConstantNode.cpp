#pragma once

#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
    void ConstantNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

	int ConstantNode::GetValue() const { return m_Value; }

	bool ConstantNode::IsEqual(const DiceAst& other) const
	{
		if (const auto* otherConst = dynamic_cast<const ConstantNode*>(&other))
		{
			return m_Value == otherConst->m_Value;
		}
		return false;
	}
}
