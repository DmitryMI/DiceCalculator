#include <gtest/gtest.h>

#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include "DiceCalculator/IRandom.h"

#include "DiceCalculator/TestUtilities.h"

#include <memory>
#include <vector>

using namespace DiceCalculator;
using namespace DiceCalculator::Expressions;
using namespace DiceCalculator::Evaluation;
using namespace DiceCalculator::Operators;
using namespace DiceCalculator::TestUtilities;

namespace DiceCalculator::Evaluation
{

	class CombinationVisitorTest : public TestHelpers
	{
	};

	TEST_F(CombinationVisitorTest, SingleConstantProducesSingleOutcome)
	{
		auto node = CreateConstant(42);
		DiceCalculator::Evaluation::CombinationAstVisitor visitor;

		node->Accept(visitor);
		const auto& outcomes = visitor.GetCombinations();

		EXPECT_EQ(outcomes.size(), 1u);
		EXPECT_EQ(outcomes[0].TotalValue, 42);
		EXPECT_EQ(outcomes[0].Rolls.size(), 0);
	}

	TEST_F(CombinationVisitorTest, SingleDiceProducesThreeOutcomes)
	{
		auto node = CreateDice(1, 3);
		DiceCalculator::Evaluation::CombinationAstVisitor visitor;

		node->Accept(visitor);
		const auto& outcomes = visitor.GetCombinations();

		EXPECT_EQ(outcomes.size(), 3u);

		EXPECT_EQ(outcomes[0].TotalValue, 1);
		EXPECT_EQ(outcomes[0].Rolls.size(), 1);
		EXPECT_EQ(outcomes[0].Rolls[0].Sides, 3);
		EXPECT_EQ(outcomes[0].Rolls[0].Value, 1);

		EXPECT_EQ(outcomes[1].TotalValue, 2);
		EXPECT_EQ(outcomes[1].Rolls.size(), 1);
		EXPECT_EQ(outcomes[1].Rolls[0].Sides, 3);
		EXPECT_EQ(outcomes[1].Rolls[0].Value, 2);

		EXPECT_EQ(outcomes[2].TotalValue, 3);
		EXPECT_EQ(outcomes[2].Rolls.size(), 1);
		EXPECT_EQ(outcomes[2].Rolls[0].Sides, 3);
		EXPECT_EQ(outcomes[2].Rolls[0].Value, 3);
	}

	TEST_F(CombinationVisitorTest, DoubleDiceProducesNineOutcomes)
	{
		auto node = CreateDice(2, 3);
		DiceCalculator::Evaluation::CombinationAstVisitor visitor;

		node->Accept(visitor);
		const auto& outcomes = visitor.GetCombinations();

		EXPECT_EQ(outcomes.size(), 9u);

		// Expected ordered outcomes: (1,1),(1,2),(1,3),(2,1),(2,2),(2,3),(3,1),(3,2),(3,3)
		const int expectedSides = 3;
		const int expectedRolls = 2;

		size_t idx = 0;
		for (int first = 1; first <= 3; ++first)
		{
			for (int second = 1; second <= 3; ++second, ++idx)
			{
				const auto& o = outcomes[idx];
				EXPECT_EQ(o.TotalValue, first + second) << "at index " << idx;
				ASSERT_EQ(o.Rolls.size(), static_cast<size_t>(expectedRolls)) << "at index " << idx;

				EXPECT_EQ(o.Rolls[0].Sides, expectedSides) << "at index " << idx << " (first roll sides)";
				EXPECT_EQ(o.Rolls[0].Value, first) << "at index " << idx << " (first roll value)";

				EXPECT_EQ(o.Rolls[1].Sides, expectedSides) << "at index " << idx << " (second roll sides)";
				EXPECT_EQ(o.Rolls[1].Value, second) << "at index " << idx << " (second roll value)";
			}
		}
	}
}