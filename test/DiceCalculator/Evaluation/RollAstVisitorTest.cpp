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
		const auto& records = visitor.GetDiceRecords();
		ASSERT_EQ(records.size(), 3); // Advantage discards inner rolls except the highest
		EXPECT_EQ(records[0].Sides, 10);
		EXPECT_EQ(records[0].RolledValue, 3);
		EXPECT_EQ(records[1].Sides, 10);
		EXPECT_EQ(records[1].RolledValue, 4);
		EXPECT_EQ(records[2].Sides, 6);
		EXPECT_EQ(records[2].RolledValue, 5);
	}

	TEST_F(RollVisitorTest, ComparisonOfTwoConstants)
	{
		MockRandom rnd({ 1, 2, 3, 4, 5 });
		RollAstVisitor visitor(rnd);

		auto node1 = CreateConstant(33);
		auto node2 = CreateConstant(44);
		auto lessNode = CreateLessThanNode(node1, node2);
		auto lessEqualNode = CreateLessThanOrEqualNode(node1, node2);
		auto equalNode = CreateEqualNode(node1, node2);
		auto notEqualNode = CreateNotEqualNode(node1, node2);
		auto greaterOrEqualNode = CreateGreaterThanOrEqualNode(node1, node2);
		auto greaterNode = CreateGreaterThanNode(node1, node2);

		lessNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);

		lessEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);

		equalNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);

		notEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);

		greaterOrEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);

		greaterNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);
	}

	TEST_F(RollVisitorTest, ComparisonOfTwoRolls)
	{
		MockRandom rnd({ 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 });
		RollAstVisitor visitor(rnd);

		auto node1 = CreateDice(1, 6);
		auto node2 = CreateDice(1, 6);
		auto lessNode = CreateLessThanNode(node1, node2);
		auto lessEqualNode = CreateLessThanOrEqualNode(node1, node2);
		auto equalNode = CreateEqualNode(node1, node2);
		auto notEqualNode = CreateNotEqualNode(node1, node2);
		auto greaterOrEqualNode = CreateGreaterThanOrEqualNode(node1, node2);
		auto greaterNode = CreateGreaterThanNode(node1, node2);

		lessNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);
		ASSERT_EQ(visitor.GetDiceRecords().size(), 2);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 6);
		EXPECT_EQ(visitor.GetDiceRecords()[1].Sides, 6);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 2);
		EXPECT_EQ(visitor.GetDiceRecords()[1].RolledValue, 1);

		lessEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);

		equalNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);

		notEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);

		greaterOrEqualNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);

		greaterNode->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);
	}

	/// <summary>
	/// Testing expression: AttackRoll(1d20 + 5 + 1d8, 15). The mocked d20 roll is 13, mocked d8 roll is 3, so total 21 vs AC 15 -> hit (1).
	/// </summary>
	TEST_F(RollVisitorTest, AttackRollNormal)
	{
		MockRandom rnd({ 13, 3});
		RollAstVisitor visitor(rnd);

		auto d20 = CreateDice(1, 20);
		auto modifier = CreateConstant(5);
		auto d8 = CreateDice(1, 8);
		auto attackOperand = CreateAdditionNode({ d20, modifier, d8 });
		auto armorClass = CreateConstant(15);
		auto attackRoll = CreateAttackRollNode(attackOperand, armorClass);

		attackRoll->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);
		ASSERT_EQ(visitor.GetDiceRecords().size(), 2);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 13);
		EXPECT_EQ(visitor.GetDiceRecords()[1].Sides, 8);
		EXPECT_EQ(visitor.GetDiceRecords()[1].RolledValue, 3);
	}

	/// <summary>
	/// Testing expression: AttackRoll(1d20 + 5 + 1d8, 5). The mocked d20 roll is 1, mocked d8 roll is 3, so total 9 vs AC 15 -> could be a hit (1), but should be crit. miss instead.
	/// </summary>
	TEST_F(RollVisitorTest, AttackRollCriticalMiss)
	{
		MockRandom rnd({ 1, 3 });
		RollAstVisitor visitor(rnd);

		auto d20 = CreateDice(1, 20);
		auto modifier = CreateConstant(5);
		auto d8 = CreateDice(1, 8);
		auto attackOperand = CreateAdditionNode({ d20, modifier, d8 });
		auto armorClass = CreateConstant(5);
		auto attackRoll = CreateAttackRollNode(attackOperand, armorClass);

		attackRoll->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);
		EXPECT_EQ(visitor.GetDiceRecords().size(), 2);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 1);
		EXPECT_EQ(visitor.GetDiceRecords()[1].Sides, 8);
		EXPECT_EQ(visitor.GetDiceRecords()[1].RolledValue, 3);
	}

	/// <summary>
	/// Testing expression: AttackRoll(1d20 - 5 - 1d8, 15). The mocked d20 roll is 20, mocked d8 roll is 3, so total 12 vs AC 15 -> could be a miss (0), but should be crit. hit instead.
	/// </summary>
	TEST_F(RollVisitorTest, AttackRollCriticalHit)
	{
		MockRandom rnd({ 20, 3 });
		RollAstVisitor visitor(rnd);

		auto d20 = CreateDice(1, 20);
		auto modifier = CreateConstant(5);
		auto d8 = CreateDice(1, 8);
		auto attackOperand = CreateSubtractionNode({ d20, modifier, d8 });
		auto armorClass = CreateConstant(15);
		auto attackRoll = CreateAttackRollNode(attackOperand, armorClass);

		attackRoll->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);
		ASSERT_EQ(visitor.GetDiceRecords().size(), 2);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[1].Sides, 8);
		EXPECT_EQ(visitor.GetDiceRecords()[1].RolledValue, 3);
	}

	/// <summary>
	/// Testing expression: AttackRoll(Adv(1d20 - 1), 17). The mocked d20 roll is (1, 18). Crit. Miss should be discarded and the attack will hit.
	/// </summary>
	TEST_F(RollVisitorTest, AttackRollAdvantageHit)
	{
		MockRandom rnd({ 1, 18 });
		RollAstVisitor visitor(rnd);

		auto d20 = CreateDice(1, 20);
		auto modifier = CreateConstant(1);
		auto attackOperand = CreateAdvantageNode(CreateSubtractionNode({ d20, modifier }));
		auto armorClass = CreateConstant(17);
		auto attackRoll = CreateAttackRollNode(attackOperand, armorClass);

		attackRoll->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 1);
		ASSERT_EQ(visitor.GetDiceRecords().size(), 1);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 18);
	}

	/// <summary>
	/// Testing expression: AttackRoll(Dis(1d20 + 1), 1). The mocked d20 roll is (20, 1). Crit. hit should be discarded and the attack will miss.
	/// </summary>
	TEST_F(RollVisitorTest, AttackRollDisadvantageMiss)
	{
		MockRandom rnd({ 1, 20 });
		RollAstVisitor visitor(rnd);

		auto d20 = CreateDice(1, 20);
		auto modifier = CreateConstant(1);
		auto attackOperand = CreateDisadvantageNode(CreateAdditionNode({ d20, modifier }));
		auto armorClass = CreateConstant(1);
		auto attackRoll = CreateAttackRollNode(attackOperand, armorClass);

		attackRoll->Accept(visitor);
		EXPECT_EQ(visitor.GetResult(), 0);
		ASSERT_EQ(visitor.GetDiceRecords().size(), 1);
		EXPECT_EQ(visitor.GetDiceRecords()[0].Sides, 20);
		EXPECT_EQ(visitor.GetDiceRecords()[0].RolledValue, 1);
	}
}