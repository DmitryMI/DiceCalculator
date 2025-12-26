#pragma once

#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include <typeindex>
#include <stdexcept>

namespace DiceCalculator::Expressions
{
    void OperatorNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }

    const std::vector<std::shared_ptr<DiceAst>>& OperatorNode::GetOperands() const { return m_Operands; }

    bool OperatorNode::IsEqual(const DiceAst& other) const
    {
        if (const auto* otherOp = dynamic_cast<const OperatorNode*>(&other))
        {
            if (m_Operands.size() != otherOp->m_Operands.size())
            {
                return false;
            }
            for (size_t i = 0; i < m_Operands.size(); ++i)
            {
                if (!m_Operands[i])
                {
					throw std::runtime_error("Operand is null in IsEqual comparison.");
                }

                if (!m_Operands[i]->IsEqual(*otherOp->m_Operands[i]))
                {
                    return false;
                }
            }
            return m_Operator->IsEqual(*otherOp->m_Operator);
        }
        return false;
	}
}
