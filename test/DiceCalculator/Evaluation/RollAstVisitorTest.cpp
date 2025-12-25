#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include "DiceCalculator/IRandom.h"

#include "DiceCalculator/Evaluation/TestUtilities.h"

#include <memory>
#include <vector>

using namespace DiceCalculator;
using namespace DiceCalculator::Expressions;
using namespace DiceCalculator::Evaluation;
using namespace DiceCalculator::Operators;
using namespace DiceCalculator::TestUtilities;

namespace DiceCalculator::Evaluation
{

	class RollVisitorTest : public TestHelpers
	{
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
		MockRandom rnd({ 4, 5 });
		RollAstVisitor visitor(rnd);

		auto node = CreateDice(2, 6);
		node->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), 9);
	}

	TEST_F(RollVisitorTest, AdditionOperatorCombinesOperands)
	{
		// One constant (3) and one dice with a single roll returning 2 -> total 5
		MockRandom rnd({ 2 });
		RollAstVisitor visitor(rnd);

		std::vector<std::shared_ptr<DiceAst>> operands;
		operands.push_back(CreateConstant(3));
		operands.push_back(CreateDice(1, 4));

		auto opNode = CreateAdditionNode(std::move(operands));
		opNode->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), 5);
	}

	TEST_F(RollVisitorTest, SubtractionOperatorCombinesOperands)
	{
		// One constant (3) and one dice with a single roll returning 2 -> total 1
		MockRandom rnd({ 2 });
		RollAstVisitor visitor(rnd);

		std::vector<std::shared_ptr<DiceAst>> operands;
		operands.push_back(CreateConstant(3));
		operands.push_back(CreateDice(1, 4));

		auto opNode = std::make_shared<OperatorNode>(std::make_shared<Subtraction>(), std::move(operands));
		opNode->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), 1);
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

	// Subtraction operator specific roll tests

	TEST_F(RollVisitorTest, SubtractionOfThreeConstantsEvaluatesCorrectly)
	{
		auto node1 = CreateConstant(3);
		auto node2 = CreateConstant(4);
		auto node3 = CreateConstant(5);
		auto subtractionNode = std::make_shared<OperatorNode>(
			std::make_shared<Subtraction>(),
			std::vector<std::shared_ptr<DiceAst>>{ node1, node2, node3 });

		MockRandom rnd({}); // not used
		RollAstVisitor visitor(rnd);

		subtractionNode->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), 3 - 4 - 5);
	}

	TEST_F(RollVisitorTest, SubtractionOfSingleDiceAndSingleConstantEvaluatesCorrectly)
	{
		// Dice returns 4, constant is 4 => result 0
		MockRandom rnd({ 4 });
		RollAstVisitor visitor(rnd);

		auto dice = CreateDice(1, 6);
		auto constant = CreateConstant(4);
		auto subtractionNode = std::make_shared<OperatorNode>(
			std::make_shared<Subtraction>(),
			std::vector<std::shared_ptr<DiceAst>>{ dice, constant });

		subtractionNode->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), 4 - 4);
	}

	TEST_F(RollVisitorTest, SubtractionOfTwoDicesEvaluatesDifference)
	{
		// First die -> 1, second die -> 6 => 1 - 6 = -5
		MockRandom rnd({ 1, 6 });
		RollAstVisitor visitor(rnd);

		auto dice1 = CreateDice(1, 6);
		auto dice2 = CreateDice(1, 6);
		auto subtractionNode = std::make_shared<OperatorNode>(
			std::make_shared<Subtraction>(),
			std::vector<std::shared_ptr<DiceAst>>{ dice1, dice2 });

		subtractionNode->Accept(visitor);

		EXPECT_EQ(visitor.GetResult(), -5);
	}

	// Complex expression: ADV(2d10 + 4) + 1d6
	TEST_F(RollVisitorTest, ComplexExpressionEvaluatesCorrectly)
	{
		MockRandom rnd({ 1, 2, 3, 4, 5 });
		RollAstVisitor visitor(rnd);

		std::shared_ptr<DiceAst> innerRoll = CreateDice(2, 10);
		std::shared_ptr<DiceAst> innerAddition = CreateAdditionNode({ innerRoll, CreateConstant(4) });
		std::shared_ptr<DiceAst> advantage = CreateAdvantageNode(innerAddition);
		std::shared_ptr<DiceAst> outerRoll = CreateDice(1, 6);
		std::shared_ptr<DiceAst> totalExpression = CreateAdditionNode({ advantage, outerRoll });

		totalExpression->Accept(visitor);

		// Should compute ADV(2d10 + 4) + 1d6 == ADV((1+2) + 4, (3+4) + 4) + 1d6 == ADV(7, 11) + 1d6 == 11 + 5 == 16
		EXPECT_EQ(visitor.GetResult(), 16);
	}
}