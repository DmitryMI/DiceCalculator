#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"
#include "DiceCalculator/Evaluation/TestUtilities.h"

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
		EXPECT_DOUBLE_EQ(dist[42], 1.0);
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
		EXPECT_DOUBLE_EQ(dist[1], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[2], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[3], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[4], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[5], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[6], 1.0 / 6);
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
		EXPECT_DOUBLE_EQ(dist[2], 1.0 / 36);
		EXPECT_DOUBLE_EQ(dist[3], 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[4], 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[5], 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[6], 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[7], 6.0 / 36);
		EXPECT_DOUBLE_EQ(dist[8], 5.0 / 36);
		EXPECT_DOUBLE_EQ(dist[9], 4.0 / 36);
		EXPECT_DOUBLE_EQ(dist[10], 3.0 / 36);
		EXPECT_DOUBLE_EQ(dist[11], 2.0 / 36);
		EXPECT_DOUBLE_EQ(dist[12], 1.0 / 36);
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
		EXPECT_DOUBLE_EQ(dist[3], 1.0 / 27);
		EXPECT_DOUBLE_EQ(dist[4], 3.0 / 27);
		EXPECT_DOUBLE_EQ(dist[5], 6.0 / 27);
		EXPECT_DOUBLE_EQ(dist[6], 7.0 / 27);
		EXPECT_DOUBLE_EQ(dist[7], 6.0 / 27);
		EXPECT_DOUBLE_EQ(dist[8], 3.0 / 27);
		EXPECT_DOUBLE_EQ(dist[9], 1.0 / 27);
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
		EXPECT_DOUBLE_EQ(dist[node1->GetValue() + node2->GetValue() + node3->GetValue()], 1.0);
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
		EXPECT_DOUBLE_EQ(dist[1 + node2->GetValue()], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[2 + node2->GetValue()], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[3 + node2->GetValue()], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[4 + node2->GetValue()], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[5 + node2->GetValue()], 1.0 / 6);
		EXPECT_DOUBLE_EQ(dist[6 + node2->GetValue()], 1.0 / 6);
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
		EXPECT_DOUBLE_EQ(dist1[2], dist2[2]);
		EXPECT_DOUBLE_EQ(dist1[3], dist2[3]);
		EXPECT_DOUBLE_EQ(dist1[4], dist2[4]);
		EXPECT_DOUBLE_EQ(dist1[5], dist2[5]);
		EXPECT_DOUBLE_EQ(dist1[6], dist2[6]);
		EXPECT_DOUBLE_EQ(dist1[7], dist2[7]);
		EXPECT_DOUBLE_EQ(dist1[8], dist2[8]);
		EXPECT_DOUBLE_EQ(dist1[9], dist2[9]);
		EXPECT_DOUBLE_EQ(dist1[10], dist2[10]);
		EXPECT_DOUBLE_EQ(dist1[11], dist2[11]);
		EXPECT_DOUBLE_EQ(dist1[12], dist2[12]);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfConstantIsSameConstant)
	{
		auto node = CreateConstant(33);
		auto advantageNode = CreateAdvantageNode(node);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 1);
		EXPECT_DOUBLE_EQ(dist[33], 1);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfSingleDice)
	{
		auto dice = CreateDice(1, 3);
		auto advantageNode = CreateAdvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[1], 1.0 / 9);
		EXPECT_DOUBLE_EQ(dist[2], 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[3], 5.0 / 9);
	}

	TEST_F(DistributionVisitorTest, AdvantageOfDoubleDice)
	{
		auto dice = CreateDice(2, 2);
		auto advantageNode = CreateAdvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[2], 1.0 / 16);
		EXPECT_DOUBLE_EQ(dist[3], 8.0 / 16);
		EXPECT_DOUBLE_EQ(dist[4], 7.0 / 16);
	}

	TEST_F(DistributionVisitorTest, DisadvantageOfSingleDice)
	{
		auto dice = CreateDice(1, 3);
		auto advantageNode = CreateDisadvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[3], 1.0 / 9);
		EXPECT_DOUBLE_EQ(dist[2], 3.0 / 9);
		EXPECT_DOUBLE_EQ(dist[1], 5.0 / 9);
	}

	TEST_F(DistributionVisitorTest, DisadvantageOfDoubleDice)
	{
		auto dice = CreateDice(2, 2);
		auto advantageNode = CreateDisadvantageNode(dice);

		DiceCalculator::Evaluation::DistributionAstVisitor visitor;

		advantageNode->Accept(visitor);
		const auto& dist = visitor.GetDistribution();

		EXPECT_EQ(dist.Size(), 3);
		EXPECT_DOUBLE_EQ(dist[4], 1.0 / 16);
		EXPECT_DOUBLE_EQ(dist[3], 8.0 / 16);
		EXPECT_DOUBLE_EQ(dist[2], 7.0 / 16);
	}
	
}