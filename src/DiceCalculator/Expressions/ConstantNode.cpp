#pragma once

#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
    void ConstantNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

	int ConstantNode::GetValue() const { return m_Value; }
}
