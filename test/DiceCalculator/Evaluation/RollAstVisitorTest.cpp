#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/IRandom.h"

#include <memory>
#include <vector>

using namespace DiceCalculator;
using namespace DiceCalculator::Expressions;
using namespace DiceCalculator::Evaluation;
using namespace DiceCalculator::Operators;

namespace
{
	// Simple deterministic IRandom for tests
	class MockRandom : public IRandom
	{
	public:
		explicit MockRandom(std::vector<int> values) : m_Values(std::move(values)), m_Index(0) {}

		int NextInt(int /*minInclusive*/, int /*maxInclusive*/) override
		{
			if (m_Index >= m_Values.size())
				return 0;
			return m_Values[m_Index++];
		}

	private:
		std::vector<int> m_Values;
		std::size_t m_Index;
	};
}

class RollVisitorTest : public ::testing::Test
{
protected:
	std::shared_ptr<ConstantNode> CreateConstant(int v) const
	{
		return std::make_shared<ConstantNode>(v);
	}

	std::shared_ptr<DiceNode> CreateDice(int rolls, int sides) const
	{
		return std::make_shared<DiceNode>(rolls, sides);
	}

	std::shared_ptr<OperatorNode> CreateAdditionNode(std::vector<std::shared_ptr<DiceAst>> operands) const
	{
		return std::make_shared<OperatorNode>(std::make_shared<Addition>(), std::move(operands));
	}

	std::shared_ptr<OperatorNode> CreateAdvantageNode(std::shared_ptr<DiceAst> operand) const
	{
		return std::make_shared<OperatorNode>(std::make_shared<Advantage>(Advantage::Mode::Advantage), std::vector<std::shared_ptr<DiceAst>>{ operand });
	}

	std::shared_ptr<OperatorNode> CreateDisadvantageNode(std::shared_ptr<DiceAst> operand) const
	{
		return std::make_shared<OperatorNode>(std::make_shared<Advantage>(Advantage::Mode::Disadvantage), std::vector<std::shared_ptr<DiceAst>>{ operand });
	}
};

TEST_F(RollVisitorTest, ConstantNodeProducesValue)
{
	MockRandom rnd({}); // not used
	RollAstVisitor visitor(rnd);

	auto node = CreateConstant(42);
	node->Accept(visitor);

	EXPECT_EQ(visitor.GetResult(), 42);
}

TEST_F(RollVisitorTest, DiceNodeRollsUsingRandom)
{
	// Two rolls that return 4 and 5 -> total 9
	MockRandom rnd({4, 5});
	RollAstVisitor visitor(rnd);

	auto node = CreateDice(2, 6);
	node->Accept(visitor);

	EXPECT_EQ(visitor.GetResult(), 9);
}

TEST_F(RollVisitorTest, AdditionOperatorCombinesOperands)
{
	// One constant (3) and one dice with a single roll returning 2 -> total 5
	MockRandom rnd({2});
	RollAstVisitor visitor(rnd);

	std::vector<std::shared_ptr<DiceAst>> operands;
	operands.push_back(CreateConstant(3));
	operands.push_back(CreateDice(1, 4));

	auto opNode = CreateAdditionNode(std::move(operands));
	opNode->Accept(visitor);

	EXPECT_EQ(visitor.GetResult(), 5);
}

TEST_F(RollVisitorTest, AdvantageOperatorTakesMaximum)
{
	// Three rolls: 2 and 13 -> advantage is 13. The last roll (3) should be ignored.
	MockRandom rnd({ 2, 13, 3 });
	RollAstVisitor visitor(rnd);

	std::shared_ptr<DiceAst> roll = CreateDice(1, 20);

	std::shared_ptr<DiceAst> advantage = CreateAdvantageNode(roll);

	advantage->Accept(visitor);

	EXPECT_EQ(visitor.GetResult(), 13);
}

TEST_F(RollVisitorTest, DisadvantageOperatorTakesMinimum)
{
	// Three rolls: 3 and 13 -> disadvantage is 3. The last roll (2) should be ignored.
	MockRandom rnd({ 3, 13, 2 });
	RollAstVisitor visitor(rnd);

	std::shared_ptr<DiceAst> roll = CreateDice(1, 20);

	std::shared_ptr<DiceAst> advantage = CreateDisadvantageNode(roll);

	advantage->Accept(visitor);

	EXPECT_EQ(visitor.GetResult(), 3);
}