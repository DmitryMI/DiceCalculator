#pragma once

#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
    void OperatorNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

    const std::vector<std::shared_ptr<DiceAst>>& OperatorNode::GetOperands() const { return m_Operands; }
}
