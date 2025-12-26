#pragma once

#include "DiceCalculator/Expressions/DiceAst.h"

namespace DiceCalculator::Expressions
{
    class ConstantNode : public DiceAst
    {
    public:

        explicit ConstantNode(int v) : m_Value(v) {}

        void Accept(DiceCalculator::Evaluation::DiceAstVisitor& v) const override;

        int GetValue() const;

        bool IsEqual(const DiceAst& other) const override;

    private:
        int m_Value;
    };

}
