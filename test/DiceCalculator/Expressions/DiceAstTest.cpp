#include <gtest/gtest.h>

#include "DiceCalculator/TestUtilities.h"

#include <memory>
#include <vector>

namespace DiceCalculator::Expressions
{
    class DiceAstTest : public TestUtilities::TestHelpers
    {
    };

    TEST_F(DiceAstTest, ConstructTest)
    {
        std::shared_ptr<DiceAst> ast = CreateAdvantageNode(
            CreateAdditionNode({
                CreateDice(2, 10),
                CreateDice(4, 20),
                CreateConstant(10)
            })
        );

        ASSERT_NE(ast, nullptr);
    }

    // ConstantNode equality
    TEST_F(DiceAstTest, ConstantNodesEqualAndNotEqual)
    {
        auto a = CreateConstant(5);
        auto b = CreateConstant(5);
        auto c = CreateConstant(6);

        EXPECT_TRUE(a->IsEqual(*b));
        EXPECT_TRUE(b->IsEqual(*a));
        EXPECT_FALSE(a->IsEqual(*c));
    }

    // DiceNode equality
    TEST_F(DiceAstTest, DiceNodesEqualAndNotEqual)
    {
        auto d1 = CreateDice(2, 6);
        auto d2 = CreateDice(2, 6);
        auto d3 = CreateDice(3, 6);
        auto d4 = CreateDice(2, 8);

        EXPECT_TRUE(d1->IsEqual(*d2));
        EXPECT_FALSE(d1->IsEqual(*d3));
        EXPECT_FALSE(d1->IsEqual(*d4));
    }

    // OperatorNode equality: same operator type and recursively-equal operands
    TEST_F(DiceAstTest, OperatorNodesEqualForSameStructure)
    {
        auto op1 = CreateAdditionNode({
            CreateDice(1, 6),
            CreateConstant(3)
        });

        auto op2 = CreateAdditionNode({
            CreateDice(1, 6),
            CreateConstant(3)
        });

        auto opDifferentOperand = CreateAdditionNode({
            CreateDice(1, 6),
            CreateConstant(4)
        });

        auto opReordered = CreateAdditionNode({
            CreateConstant(3),
            CreateDice(1, 6)
        });

        auto opSub = CreateSubtractionNode({
            CreateDice(1, 6),
            CreateConstant(3)
        });

        EXPECT_TRUE(op1->IsEqual(*op2));
        EXPECT_FALSE(op1->IsEqual(*opDifferentOperand));
        EXPECT_FALSE(op1->IsEqual(*opReordered)); // operand order matters
        EXPECT_FALSE(op1->IsEqual(*opSub));       // different operator type
    }

    // OperatorNode equality for stateful operator types (current implementation compares type only)
    TEST_F(DiceAstTest, AdvantageOperatorComparison)
    {
        auto adv1 = CreateAdvantageNode(CreateDice(1, 20));
        auto adv2 = CreateAdvantageNode(CreateDice(1, 20));

        EXPECT_TRUE(adv1->IsEqual(*adv2));
    }
}