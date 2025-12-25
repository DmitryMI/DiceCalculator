#pragma once

#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
    void OperatorNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

	const std::string& OperatorNode::GetSymbol() const { return m_Symbol; }

    const std::vector<std::shared_ptr<DiceAst>>& OperatorNode::GetOperands() const { return m_Operands; }
}
