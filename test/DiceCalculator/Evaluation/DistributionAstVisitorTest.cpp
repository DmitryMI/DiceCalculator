#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"
#include "DiceCalculator/Evaluation/TestUtilities.h"

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