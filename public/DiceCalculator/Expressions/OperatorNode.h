#pragma once

#include <string>
#include <vector>
#include <memory>
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"
#include "DiceCalculator/Operators/DiceOperator.h"

namespace DiceCalculator::Expressions
{
    class OperatorNode : public DiceAst
    {
    public:
        
        OperatorNode(std::shared_ptr<DiceCalculator::Operators::DiceOperator> op,
            std::vector<std::shared_ptr<DiceAst>> ops)
            : m_Operator(std::move(op)), m_Operands(std::move(ops))
        {
        }

        void Accept(Evaluation::DiceAstVisitor& v) const override;

        const std::vector<std::shared_ptr<DiceAst>>& GetOperands() const;
		const std::shared_ptr<DiceCalculator::Operators::DiceOperator>& GetOperator() const { return m_Operator; }
        bool IsEqual(const DiceAst& other) const override;

    private:
        std::shared_ptr<DiceCalculator::Operators::DiceOperator> m_Operator;
        std::vector<std::shared_ptr<DiceAst>> m_Operands;

    };

}