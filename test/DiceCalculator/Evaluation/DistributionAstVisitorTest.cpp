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
}