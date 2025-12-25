#pragma once

#include <string>
#include <vector>
#include <memory>
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
    class OperatorNode : public DiceAst
    {
    public:
        
        OperatorNode(std::string sym,
            std::vector<std::shared_ptr<DiceAst>> ops)
            : m_Symbol(std::move(sym)), m_Operands(std::move(ops))
        {
        }

        void Accept(Evaluation::DiceAstVisitor& v) const override;

        const std::string& GetSymbol() const;

        const std::vector<std::shared_ptr<DiceAst>>& GetOperands() const;

    private:
        std::string m_Symbol;
        std::vector<std::shared_ptr<DiceAst>> m_Operands;

    };

}