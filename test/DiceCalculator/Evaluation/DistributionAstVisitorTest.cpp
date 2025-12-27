#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"
#include "DiceCalculator/TestUtilities.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include <array>

namespace DiceCalculator::Evaluation
{
	using namespace DiceCalculator::TestUtilities;

	class DistributionVisitorTest : public TestHelpers
	{
	};

	TEST_F(DistributionVisitorTest, SingleConstantProducesDistribution)
	{
		// Arrange
		auto node = CreateConstant(42);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		EXPECT_EQ(dist.Size(), 1u);
		EXPECT_DOUBLE_EQ(dist[42].Probability, 1.0);
		EXPECT_EQ(dist[42].Value, 42);
		EXPECT_EQ(dist[42].D20, 0);
	}

	TEST_F(DistributionVisitorTest, SingleDiceRollProducesUniformDistribution)
	{
		// Arrange
		auto node = CreateDice(1, 6);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		EXPECT_EQ(dist.Size(), 6u);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[5].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[6].Probability, 1.0 / 6);

		EXPECT_DOUBLE_EQ(dist[1].D20, 0);
		EXPECT_DOUBLE_EQ(dist[2].D20, 0);
		EXPECT_DOUBLE_EQ(dist[3].D20, 0);
		EXPECT_DOUBLE_EQ(dist[4].D20, 0);
		EXPECT_DOUBLE_EQ(dist[5].D20, 0);
		EXPECT_DOUBLE_EQ(dist[6].D20, 0);
	}

	TEST_F(DistributionVisitorTest, SingleD20RollProducesUniformDistributionAndTags)
	{
		// Arrange
		auto node = CreateDice(1, 20);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		EXPECT_EQ(dist.Size(), 20);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[5].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[6].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[7].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[8].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[9].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[10].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[11].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[12].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[13].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[14].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[15].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[16].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[17].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[18].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[19].Probability, 1.0 / 20);
		EXPECT_DOUBLE_EQ(dist[20].Probability, 1.0 / 20);

		EXPECT_EQ(dist[1].D20, 1);
		EXPECT_EQ(dist[2].D20, 2);
		EXPECT_EQ(dist[3].D20, 3);
		EXPECT_EQ(dist[4].D20, 4);
		EXPECT_EQ(dist[5].D20, 5);
		EXPECT_EQ(dist[6].D20, 6);
		EXPECT_EQ(dist[7].D20, 7);
		EXPECT_EQ(dist[8].D20, 8);
		EXPECT_EQ(dist[9].D20, 9);
		EXPECT_EQ(dist[10].D20, 10);
		EXPECT_EQ(dist[11].D20, 11);
		EXPECT_EQ(dist[12].D20, 12);
		EXPECT_EQ(dist[13].D20, 13);
		EXPECT_EQ(dist[14].D20, 14);
		EXPECT_EQ(dist[15].D20, 15);
		EXPECT_EQ(dist[16].D20, 16);
		EXPECT_EQ(dist[17].D20, 17);
		EXPECT_EQ(dist[18].D20, 18);
		EXPECT_EQ(dist[19].D20, 19);
		EXPECT_EQ(dist[20].D20, 20);
	}

	TEST_F(DistributionVisitorTest, TwoD20TracksFirstDieOnly)
	{
		// Arrange
		auto node = CreateDice(2, 20);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		// 2d20 ranges from 2..40 (39 outcomes)
		EXPECT_EQ(dist.Size(), 39u);

		// Sums up to 21: the highest possible first die for a given sum is (sum - 1)
		EXPECT_EQ(dist[2].D20, 1);   // only 1+1
		EXPECT_EQ(dist[3].D20, 2);   // 1+2 or 2+1 -> max first die is 2
		EXPECT_EQ(dist[10].D20, 9);  // max first die is 9
		EXPECT_EQ(dist[20].D20, 19); // max first die is 19
		EXPECT_EQ(dist[21].D20, 20); // max first die is 20

		// Sums above 21: first die is capped at 20
		EXPECT_EQ(dist[22].D20, 20);
		EXPECT_EQ(dist[30].D20, 20);
		EXPECT_EQ(dist[39].D20, 20);
		EXPECT_EQ(dist[40].D20, 20);
	}

	TEST_F(DistributionVisitorTest, AdvantageDiscardsLesserRolls)
	{
		// Arrange
		auto dice = CreateDice(1, 20);
		auto adv = CreateAdvantageNode(dice); // default: 2 rolls, take the best
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		adv->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		// Advantage over 1d20 still yields 20 distinct outcomes.
		EXPECT_EQ(dist.Size(), 20u);

		// Only the all-ones case should carry D20 == 1 (probability (1/20)^2).
		std::size_t d20EqualsOneCount = 0;
		for (const auto& outcome : dist.GetData())
		{
			if (outcome.D20 == 1)
			{
				++d20EqualsOneCount;
				EXPECT_EQ(outcome.Value, 1);
				EXPECT_DOUBLE_EQ(outcome.Probability, 1.0 / 400.0);
			}
			else
			{
				EXPECT_NE(outcome.Value, 1);
			}
		}
		EXPECT_EQ(d20EqualsOneCount, 1u);
	}


	TEST_F(DistributionVisitorTest, AdvantagePlusConstKeepsInnerD20)
	{
		// Expression: ADV(5 + 1d20) -- only the inner 1d20 should set D20 tags.
		auto innerSum = CreateAdditionNode({ CreateConstant(5), CreateDice(1, 20) });
		auto fullExpr = CreateAdvantageNode(innerSum); // default: 2 rolls, take the best

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		fullExpr->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Advantage over a single d20 still yields 20 outcomes (shifted by +5 on value).
		EXPECT_EQ(dist.Size(), 20u);

		for (int face = 1; face <= 20; ++face)
		{
			const int shiftedValue = 5 + face;
			const double expectedProb = static_cast<double>(2 * face - 1) / 400.0; // P(max = face) with 2 rolls

			const auto outcome = dist[shiftedValue];
			EXPECT_EQ(outcome.D20, face) << "D20 tag should equal the d20 roll, not the summed value.";
			EXPECT_NEAR(outcome.Probability, expectedProb, 0.001);
		}
	}

	TEST_F(DistributionVisitorTest, DisadvantageKeepsGreaterRollsOnlyWhenAllMax)
	{
		// Arrange
		auto dice = CreateDice(1, 20);
		auto dis = CreateDisadvantageNode(dice); // default: 2 rolls, take the worst
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		dis->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		// Disadvantage over 1d20 still yields 20 distinct outcomes.
		EXPECT_EQ(dist.Size(), 20u);

		// Only the all-20s case should carry D20 == 20 (probability (1/20)^2).
		std::size_t d20EqualsTwentyCount = 0;
		for (const auto& outcome : dist.GetData())
		{
			if (outcome.D20 == 20)
			{
				++d20EqualsTwentyCount;
				EXPECT_EQ(outcome.Value, 20);
				EXPECT_DOUBLE_EQ(outcome.Probability, 1.0 / 400.0);
			}
			else
			{
				EXPECT_NE(outcome.Value, 20);
			}
		}
		EXPECT_EQ(d20EqualsTwentyCount, 1u);
	}

	TEST_F(DistributionVisitorTest, DoubleDiceRollProducesNormalDistribution)
	{
		// Arrange
		auto node = CreateDice(2, 6);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		EXPECT_EQ(dist.Size(), 11u);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 1.0 / 36);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[5].Probability, 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[6].Probability, 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[7].Probability, 6.0 / 36);
		EXPECT_DOUBLE_EQ(dist[8].Probability, 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[9].Probability, 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[10].Probability, 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[11].Probability, 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[12].Probability, 1.0 / 36);

		EXPECT_DOUBLE_EQ(dist[2].D20, 0);
		EXPECT_DOUBLE_EQ(dist[3].D20, 0);
		EXPECT_DOUBLE_EQ(dist[4].D20, 0);
		EXPECT_DOUBLE_EQ(dist[5].D20, 0);
		EXPECT_DOUBLE_EQ(dist[6].D20, 0);
		EXPECT_DOUBLE_EQ(dist[7].D20, 0);
		EXPECT_DOUBLE_EQ(dist[8].D20, 0);
		EXPECT_DOUBLE_EQ(dist[9].D20, 0);
		EXPECT_DOUBLE_EQ(dist[10].D20, 0);
		EXPECT_DOUBLE_EQ(dist[11].D20, 0);
		EXPECT_DOUBLE_EQ(dist[12].D20, 0);
	}

	TEST_F(DistributionVisitorTest, ThreeDiceThreeSidesProducesNormalDistribution)
	{
		// Arrange: 3d3
		auto node = CreateDice(3, 3);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		// Act
		node->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Assert
		// Possible sums: 3..9 (7 values). Counts: 1,3,6,7,6,3,1 out of 27.
		EXPECT_EQ(dist.Size(), 7u);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 1.0 / 27);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 3.0 / 27);
		EXPECT_DOUBLE_EQ(dist[5].Probability, 6.0 / 27);
		EXPECT_DOUBLE_EQ(dist[6].Probability, 7.0 / 27);
		EXPECT_DOUBLE_EQ(dist[7].Probability, 6.0 / 27);
		EXPECT_DOUBLE_EQ(dist[8].Probability, 3.0 / 27);
		EXPECT_DOUBLE_EQ(dist[9].Probability, 1.0 / 27);
	}

	TEST_F(DistributionVisitorTest, SumOfThreeConstantsProducesSingleValueDistribution)
	{
		auto node1 = CreateConstant(3);
		auto node2 = CreateConstant(4);
		auto node3 = CreateConstant(5);
		auto additionNode = CreateAdditionNode({ node1, node2, node3 });
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		additionNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 1);
		EXPECT_DOUBLE_EQ(dist[node1->GetValue() + node2->GetValue() + node3->GetValue()].Probability, 1.0);
	}

	TEST_F(DistributionVisitorTest, SumOfSingleDiceAndSingleConstantProducesUniformDistribution)
	{
		auto node1 = CreateDice(1, 6);
		auto node2 = CreateConstant(4);
		auto additionNode = CreateAdditionNode({ node1, node2 });
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		additionNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 6u);
		EXPECT_DOUBLE_EQ(dist[1 + node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[2 + node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[3 + node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[4 + node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[5 + node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[6 + node2->GetValue()].Probability, 1.0 / 6);
	}

	TEST_F(DistributionVisitorTest, SumOfTwoDicesIsEqualToDoubleDice)
	{
		auto dice1 = CreateDice(1, 6);
		auto dice2 = CreateDice(1, 6);
		auto additionNode = CreateAdditionNode({ dice1, dice2 });

		auto twoDices = CreateDice(2, 6);
		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		additionNode->Accept(visitor);
		const auto& dist1 = visitor.GetDistribution();

		twoDices->Accept(visitor);
		const auto& dist2 = visitor.GetDistribution();

		EXPECT_EQ(dist1.Size(), dist2.Size());
		EXPECT_DOUBLE_EQ(dist1[2].Probability, dist2[2].Probability);
		EXPECT_DOUBLE_EQ(dist1[3].Probability, dist2[3].Probability);
		EXPECT_DOUBLE_EQ(dist1[4].Probability, dist2[4].Probability);
		EXPECT_DOUBLE_EQ(dist1[5].Probability, dist2[5].Probability);
		EXPECT_DOUBLE_EQ(dist1[6].Probability, dist2[6].Probability);
		EXPECT_DOUBLE_EQ(dist1[7].Probability, dist2[7].Probability);
		EXPECT_DOUBLE_EQ(dist1[8].Probability, dist2[8].Probability);
		EXPECT_DOUBLE_EQ(dist1[9].Probability, dist2[9].Probability);
		EXPECT_DOUBLE_EQ(dist1[10].Probability, dist2[10].Probability);
		EXPECT_DOUBLE_EQ(dist1[11].Probability, dist2[11].Probability);
		EXPECT_DOUBLE_EQ(dist1[12].Probability, dist2[12].Probability);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfConstantIsSameConstant)
	{
		auto node = CreateConstant(33);
		auto advantageNode = CreateAdvantageNode(node);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 1);
		EXPECT_DOUBLE_EQ(dist[33].Probability, 1);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfSingleDice)
	{
		auto dice = CreateDice(1, 3);
		auto advantageNode = CreateAdvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 9);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 5.0 / 9);
	}

	TEST_F(DistributionVisitorTest, Advantage3OfSingleDice)
	{
		auto dice = CreateDice(1, 3);
		auto advantageNode = CreateAdvantageNode(dice, 3);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 27);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 7.0 / 27);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 19.0 / 27);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfSingleDicePlusConstant)
	{
		auto dice = CreateDice(1, 3);
		auto nodeConst = CreateConstant(4);
		auto additionNode = CreateAdditionNode({ dice, nodeConst });
		auto advantageNode = CreateAdvantageNode(additionNode);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[1 + nodeConst->GetValue()].Probability, 1.0 / 9);
		EXPECT_DOUBLE_EQ(dist[2 + nodeConst->GetValue()].Probability, 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[3 + nodeConst->GetValue()].Probability, 5.0 / 9);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfDoubleDice)
	{
		auto dice = CreateDice(2, 2);
		auto advantageNode = CreateAdvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 1.0 / 16);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 8.0 / 16);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 7.0 / 16);
	}

	TEST_F(DistributionVisitorTest, DisadvantageOfSingleDice)
	{
		auto dice = CreateDice(1, 3);
		auto advantageNode = CreateDisadvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 1.0 / 9);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 5.0 / 9);
	}

	TEST_F(DistributionVisitorTest, DisadvantageOfDoubleDice)
	{
		auto dice = CreateDice(2, 2);
		auto advantageNode = CreateDisadvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 1.0 / 16);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 8.0 / 16);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 7.0 / 16);
	}
	
	// --- Subtraction operator tests (mirrors addition tests) ---

	TEST_F(DistributionVisitorTest, SubtractionOfThreeConstantsProducesSingleValueDistribution)
	{
		auto node1 = CreateConstant(3);
		auto node2 = CreateConstant(4);
		auto node3 = CreateConstant(5);
		auto subtractionNode = std::make_shared<DiceCalculator::Expressions::OperatorNode>(
			std::make_shared<DiceCalculator::Operators::Subtraction>(),
			std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>>{ node1, node2, node3 });

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		subtractionNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 1u);
		EXPECT_DOUBLE_EQ(dist[node1->GetValue() - node2->GetValue() - node3->GetValue()].Probability, 1.0);
	}

	TEST_F(DistributionVisitorTest, SubtractionOfSingleDiceAndSingleConstantProducesUniformDistribution)
	{
		auto node1 = CreateDice(1, 6);
		auto node2 = CreateConstant(4);
		auto subtractionNode = std::make_shared<DiceCalculator::Expressions::OperatorNode>(
			std::make_shared<DiceCalculator::Operators::Subtraction>(),
			std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>>{ node1, node2 });

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		subtractionNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 6u);
		EXPECT_DOUBLE_EQ(dist[1 - node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[2 - node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[3 - node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[4 - node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[5 - node2->GetValue()].Probability, 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[6 - node2->GetValue()].Probability, 1.0 / 6);
	}

	TEST_F(DistributionVisitorTest, SubtractionOfTwoSingleDicesProducesDifferenceDistribution)
	{
		auto dice1 = CreateDice(1, 6);
		auto dice2 = CreateDice(1, 6);
		auto subtractionNode = std::make_shared<DiceCalculator::Expressions::OperatorNode>(
			std::make_shared<DiceCalculator::Operators::Subtraction>(),
			std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>>{ dice1, dice2 });

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		subtractionNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		// Difference range: -5..5 (11 values). Counts: 1,2,3,4,5,6,5,4,3,2,1 out of 36
		EXPECT_EQ(dist.Size(), 11u);
		EXPECT_DOUBLE_EQ(dist[-5].Probability, 1.0 / 36);
		EXPECT_DOUBLE_EQ(dist[-4].Probability, 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[-3].Probability, 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[-2].Probability, 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[-1].Probability, 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 6.0 / 36);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[2].Probability, 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[3].Probability, 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[4].Probability, 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[5].Probability, 1.0 / 36);
	}

	TEST_F(DistributionVisitorTest, CompareTwoConstants)
	{
		auto node1 = CreateConstant(6);
		auto node2 = CreateConstant(5);
		auto lessNode = CreateLessThanNode(node1, node2);
		auto greaterNode = CreateGreaterThanNode(node1, node2);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		lessNode->Accept(visitor);
		const auto& dist1 = visitor.GetDistribution();
		EXPECT_EQ(dist1.Size(), 1);
		EXPECT_DOUBLE_EQ(dist1[0].Probability, 1);

		greaterNode->Accept(visitor);
		const auto& dist2 = visitor.GetDistribution();
		ASSERT_EQ(dist2.Size(), 1);
		EXPECT_DOUBLE_EQ(dist2[1].Probability, 1);
	}

	TEST_F(DistributionVisitorTest, CompareTwoRolls)
	{
		auto node1 = CreateDice(1, 3);
		auto node2 = CreateDice(1, 3);
		auto lessNode = CreateLessThanNode(node1, node2);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		lessNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 6.0 / 9);
	}

	TEST_F(DistributionVisitorTest, AttackRollNormal)
	{
		auto node1 = CreateDice(1, 20);
		auto armorClass = CreateConstant(10);
		auto attackRoll = CreateAttackRollNode(node1, armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 9.0 / 20);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 11.0 / 20);
	}

	TEST_F(DistributionVisitorTest, AttackRollOnlyCritHitWillHit)
	{
		auto node1 = CreateDice(1, 20);
		auto armorClass = CreateConstant(30);
		auto attackRoll = CreateAttackRollNode(node1, armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 19.0 / 20);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 20);
	}

	TEST_F(DistributionVisitorTest, AttackRollOnlyCritMissWillMiss)
	{
		auto node1 = CreateDice(1, 20);
		auto armorClass = CreateConstant(1);
		auto attackRoll = CreateAttackRollNode(node1, armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 19.0 / 20);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 1.0 / 20);
	}

	TEST_F(DistributionVisitorTest, AttackRollLowAcWithAdvantage)
	{
		auto node1 = CreateDice(1, 20);
		auto armorClass = CreateConstant(1);
		auto attackRoll = CreateAttackRollNode(CreateAdvantageNode(node1), armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 1.0 / 20 / 20);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1 - dist[0].Probability);
	}

	TEST_F(DistributionVisitorTest, AttackRollHighAcWithDisadvantage)
	{
		auto node1 = CreateDice(1, 20);
		auto armorClass = CreateConstant(30);
		auto attackRoll = CreateAttackRollNode(CreateDisadvantageNode(node1), armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 2);
		EXPECT_DOUBLE_EQ(dist[1].Probability, 1.0 / 20 / 20);
		EXPECT_DOUBLE_EQ(dist[0].Probability, 1 - dist[1].Probability);
	}

	// Expression AttackRoll(1d20 + 1d20, 5) must throw an exception
	TEST_F(DistributionVisitorTest, AttackRollMultipleD20s)
	{
		auto armorClass = CreateConstant(5);
		auto attackRoll = CreateAttackRollNode(CreateAdditionNode({ CreateDice(1, 20), CreateDice(1, 20) }), armorClass);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		EXPECT_THROW(attackRoll->Accept(visitor), std::runtime_error);
	}

	// Expression 1d20 + AttackRoll(1d8 + 1d20 + 5, 5) must work normally
	TEST_F(DistributionVisitorTest, AttackRollMultipleRollsInside)
	{
		auto armorClass = CreateConstant(5);
		auto attackRoll = CreateAttackRollNode(CreateAdditionNode({ CreateDice(1, 8), CreateDice(1, 20), CreateConstant(5)}), armorClass);
		auto totalExpr = CreateAdditionNode({ CreateDice(1, 20), attackRoll });

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;
		attackRoll->Accept(visitor);
		auto dist1 = visitor.GetDistribution();

		
		totalExpr->Accept(visitor);
		auto dist2 = visitor.GetDistribution();
		EXPECT_EQ(dist2.Size(), 20);
	}
}

