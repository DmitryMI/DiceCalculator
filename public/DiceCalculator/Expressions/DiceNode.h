#pragma once

#include "DiceCalculator/Expressions/DiceAst.h"

namespace DiceCalculator::Expressions
{
    class DiceNode : public DiceAst
    {
    public:

        explicit DiceNode(int rolls, int sides) : m_Rolls(rolls), m_Sides(sides) {}

        int GetRolls() const { return m_Rolls; }
        int GetSides() const { return m_Sides; }

        void Accept(DiceCalculator::Evaluation::DiceAstVisitor& v) const override;
		
    private:
        int m_Rolls;
        int m_Sides;
    };

}