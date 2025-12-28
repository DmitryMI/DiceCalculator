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

	TEST_F(CombinationVisitorTest, AdditionOfConstants_33_plus_22_ProducesSingleCombinationWith55)
	{
		auto c33 = CreateConstant(33);
		auto c22 = CreateConstant(22);

		// Build "33 + 22"
		auto add = std::make_shared<Addition>();
		auto expr = std::make_shared<OperatorNode>(add, std::vector<std::shared_ptr<DiceAst>>{ c33, c22 });

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		expr->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		ASSERT_EQ(combinations.size(), 1u);
		EXPECT_EQ(combinations[0].TotalValue, 55);
		EXPECT_TRUE(combinations[0].Rolls.empty());
	}

	TEST_F(CombinationVisitorTest, AdditionOfDice_2d3_plus_1d6_Produces54CombinationsInDeterministicOrder)
	{
		auto d2d3 = CreateDice(2, 3);
		auto d1d6 = CreateDice(1, 6);

		// Build "2d3 + 1d6"
		auto add = std::make_shared<Addition>();
		auto expr = std::make_shared<OperatorNode>(add, std::vector<std::shared_ptr<DiceAst>>{ d2d3, d1d6 });

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		expr->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// 2d3 yields 9 combinations, 1d6 yields 6 combinations => 54 total
		ASSERT_EQ(combinations.size(), 54u);

		// Expected order from implementation:
		// For each 2d3 combo in order ( (1,1),(1,2),(1,3),(2,1),(2,2),(2,3),(3,1),(3,2),(3,3) )
		// pair with 1d6 values 1..6
		size_t idx = 0;
		for (int first = 1; first <= 3; ++first)
		{
			for (int second = 1; second <= 3; ++second)
			{
				for (int third = 1; third <= 6; ++third, ++idx)
				{
					const auto& c = combinations[idx];
					ASSERT_EQ(c.Rolls.size(), 3u) << "at index " << idx;

					// Total value matches sum of all faces
					EXPECT_EQ(c.TotalValue, first + second + third) << "at index " << idx;

					// First two rolls are from 2d3
					EXPECT_EQ(c.Rolls[0].Sides, 3) << "at index " << idx << " (first roll sides)";
					EXPECT_EQ(c.Rolls[0].Value, first) << "at index " << idx << " (first roll value)";

					EXPECT_EQ(c.Rolls[1].Sides, 3) << "at index " << idx << " (second roll sides)";
					EXPECT_EQ(c.Rolls[1].Value, second) << "at index " << idx << " (second roll value)";

					// Third roll is from 1d6
					EXPECT_EQ(c.Rolls[2].Sides, 6) << "at index " << idx << " (third roll sides)";
					EXPECT_EQ(c.Rolls[2].Value, third) << "at index " << idx << " (third roll value)";
				}
			}
		}
	}

	TEST_F(CombinationVisitorTest, SubtractionOfConstants_44_minus_33_ProducesSingleCombinationWith11)
	{
		auto c44 = CreateConstant(44);
		auto c33 = CreateConstant(33);

		// Build "44 - 33"
		auto sub = std::make_shared<Subtraction>();
		auto expr = std::make_shared<OperatorNode>(sub, std::vector<std::shared_ptr<DiceAst>>{ c44, c33 });

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		expr->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		ASSERT_EQ(combinations.size(), 1u);
		EXPECT_EQ(combinations[0].TotalValue, 11);
		EXPECT_TRUE(combinations[0].Rolls.empty());
	}

	TEST_F(CombinationVisitorTest, Mixed_AddAndSubtract_1d6_plus_3_minus_1d3_Produces18CombinationsInDeterministicOrder)
	{
		auto d1d6 = CreateDice(1, 6);
		auto c3 = CreateConstant(3);
		auto d1d3 = CreateDice(1, 3);

		// Build "(1d6 + 3) - 1d3"
		auto add = std::make_shared<Addition>();
		auto left = std::make_shared<OperatorNode>(add, std::vector<std::shared_ptr<DiceAst>>{ d1d6, c3 });

		auto sub = std::make_shared<Subtraction>();
		auto expr = std::make_shared<OperatorNode>(sub, std::vector<std::shared_ptr<DiceAst>>{ left, d1d3 });

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		expr->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// Left (1d6 + 3) yields 6 combinations; right (1d3) yields 3 combinations => 18 total
		ASSERT_EQ(combinations.size(), 18u);

		// Expected order:
		// For each left combo in order with 1d6 values 1..6 (rolls: [6:value]),
		// combine with right 1d3 values 1..3, subtracting their totals.
		size_t idx = 0;
		for (int six = 1; six <= 6; ++six)
		{
			for (int three = 1; three <= 3; ++three, ++idx)
			{
				const auto& c = combinations[idx];

				// Rolls: first from 1d6 (left), then from 1d3 (right). Constant 3 contributes no rolls.
				ASSERT_EQ(c.Rolls.size(), 2u) << "at index " << idx;

				// Total value is six + 3 - three
				EXPECT_EQ(c.TotalValue, six + 3 - three) << "at index " << idx;

				// First roll from 1d6
				EXPECT_EQ(c.Rolls[0].Sides, 6) << "at index " << idx << " (first roll sides)";
				EXPECT_EQ(c.Rolls[0].Value, six) << "at index " << idx << " (first roll value)";

				// Second roll from 1d3
				EXPECT_EQ(c.Rolls[1].Sides, 3) << "at index " << idx << " (second roll sides)";
				EXPECT_EQ(c.Rolls[1].Value, three) << "at index " << idx << " (second roll value)";
			}
		}
	}

	TEST_F(CombinationVisitorTest, Comparison_GreaterThanOrEqual_1d3_plus_1_ge_1d3_Produces9Combinations)
	{
		DiceCalculator::Evaluation::CombinationAstVisitor visitor;

		// Build left: (1d3 + 1)
		auto d1d3Left = CreateDice(1, 3);
		auto c1 = CreateConstant(1);
		auto addition = CreateAdditionNode({ d1d3Left, c1 });

		// Build right: 1d3
		auto d1d3Right = CreateDice(1, 3);

		// Comparison: (1d3 + 1) >= (1d3)
		auto expr = CreateGreaterThanOrEqualNode(addition, d1d3Right);

		expr->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// Left has 3 combos (totals 2..4), right has 3 combos (totals 1..3) => 9 total
		ASSERT_EQ(combinations.size(), 9u);

		// Expected order:
		// Left d1d3 value 1..3, each paired with right d1d3 value 1..3
		size_t idx = 0;
		for (int leftFace = 1; leftFace <= 3; ++leftFace)
		{
			for (int rightFace = 1; rightFace <= 3; ++rightFace, ++idx)
			{
				const auto& c = combinations[idx];

				// Result TotalValue is 1 when (leftFace + 1) >= rightFace, else 0
				const int leftTotal = leftFace + 1;
				const int expected = (leftTotal >= rightFace) ? 1 : 0;
				EXPECT_EQ(c.TotalValue, expected) << "at index " << idx;

				// Rolls are concatenated: first from left 1d3, then from right 1d3 (constant contributes no rolls)
				ASSERT_EQ(c.Rolls.size(), 2u) << "at index " << idx;

				EXPECT_EQ(c.Rolls[0].Sides, 3) << "at index " << idx << " (left 1d3 sides)";
				EXPECT_EQ(c.Rolls[0].Value, leftFace) << "at index " << idx << " (left 1d3 value)";

				EXPECT_EQ(c.Rolls[1].Sides, 3) << "at index " << idx << " (right 1d3 sides)";
				EXPECT_EQ(c.Rolls[1].Value, rightFace) << "at index " << idx << " (right 1d3 value)";
			}
		}
	}

	TEST_F(CombinationVisitorTest, Advantage_Function_ADV_5_ProducesSingleCombinationWith5_AndNoRolls)
	{
		// Build ADV(5) where operand is a constant
		auto c5 = CreateConstant(5);
		// Default rerolls = 2; constants have no rolls, and advantage should select the best attempt (still 5)
		auto adv = CreateAdvantageNode(c5 /*operand*/, /*rerolls*/ CreateConstant(2));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		adv->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// Base combinations for constant(5) => single combo; with advantage over constants, still single combo
		ASSERT_EQ(combinations.size(), 1u);
		EXPECT_EQ(combinations[0].TotalValue, 5);
		EXPECT_TRUE(combinations[0].Rolls.empty());
	}

	TEST_F(CombinationVisitorTest, Advantage_Function_ADV_1d6_Produces36CombinationsRecordingOnlyBestAttempt)
	{
		// Build ADV(1d6) with default rerolls = 2
		auto d1d6 = CreateDice(1, 6);
		auto adv = CreateAdvantageNode(d1d6 /*operand*/, /*rerolls*/ CreateConstant(2));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		adv->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// Base combinations for 1d6 => 6;
		// With 2 rerolls, cartesian products => 6 * 6 = 36 output combinations.
		ASSERT_EQ(combinations.size(), 36u);

		// Ordering from implementation (odometer where first index is fastest):
		// inner loop: first attempt value 1..6
		// outer loop: second attempt value 1..6
		size_t idx = 0;
		for (int second = 1; second <= 6; ++second)
		{
			for (int first = 1; first <= 6; ++first, ++idx)
			{
				const auto& c = combinations[idx];

				// Advantage selects the maximum of first and second attempts
				const int chosen = std::max(first, second);

				// Only the chosen attempt's roll should be recorded
				ASSERT_EQ(c.Rolls.size(), 1u) << "at index " << idx;
				EXPECT_EQ(c.TotalValue, chosen) << "at index " << idx;
				EXPECT_EQ(c.Rolls[0].Sides, 6) << "at index " << idx << " (chosen roll sides)";
				EXPECT_EQ(c.Rolls[0].Value, chosen) << "at index " << idx << " (chosen roll value)";
			}
		}
	}

	TEST_F(CombinationVisitorTest, Advantage_Function_ADV_1d3_plus_1d2_plus_1_Produces36CombinationsRecordingOnlyBestAttempt)
	{
		// Build ADV(1d3 + 1d2 + 1) with default rerolls = 2
		auto d1d3 = CreateDice(1, 3);
		auto d1d2 = CreateDice(1, 2);
		auto c1 = CreateConstant(1);
		auto sum = CreateAdditionNode({ d1d3, d1d2, c1 });
		auto adv = CreateAdvantageNode(sum /*operand*/, /*rerolls*/ CreateConstant(2));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		adv->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();

		// Base combinations for (1d3 + 1d2 + 1):
		// Order from Addition: (d3=1,d2=1),(1,2),(2,1),(2,2),(3,1),(3,2) => 6 combos
		// With advantage over 2 attempts, cartesian products => 6 * 6 = 36 output combinations.
		ASSERT_EQ(combinations.size(), 36u);

		// Helper to decode base-combo index into (d3, d2) faces using the known ordering
		auto decode_faces = [](int comboIndex)
			{
				int d3 = 1 + (comboIndex / 2);   // 0..1 => d3=1, 2..3 => d3=2, 4..5 => d3=3
				int d2 = 1 + (comboIndex % 2);   // even => d2=1, odd => d2=2
				return std::pair<int, int>{ d3, d2 };
			};

		// Ordering from implementation (odometer where first attempt index is fastest):
		// inner loop: first attempt combo index 0..5
		// outer loop: second attempt combo index 0..5
		size_t idx = 0;
		for (int secondCombo = 0; secondCombo < 6; ++secondCombo)
		{
			auto [secondD3, secondD2] = decode_faces(secondCombo);
			int secondTotal = secondD3 + secondD2 + 1;

			for (int firstCombo = 0; firstCombo < 6; ++firstCombo, ++idx)
			{
				const auto& c = combinations[idx];
				auto [firstD3, firstD2] = decode_faces(firstCombo);
				int firstTotal = firstD3 + firstD2 + 1;

				// Advantage selects the attempt with the higher total
				const bool firstIsChosen = firstTotal >= secondTotal;
				const int chosenD3 = firstIsChosen ? firstD3 : secondD3;
				const int chosenD2 = firstIsChosen ? firstD2 : secondD2;
				const int chosenTotal = chosenD3 + chosenD2 + 1;

				// Only the chosen attempt's rolls should be recorded: two rolls (d3, d2)
				ASSERT_EQ(c.Rolls.size(), 2u) << "at index " << idx;
				EXPECT_EQ(c.TotalValue, chosenTotal) << "at index " << idx;

				EXPECT_EQ(c.Rolls[0].Sides, 3) << "at index " << idx << " (chosen d3 sides)";
				EXPECT_EQ(c.Rolls[0].Value, chosenD3) << "at index " << idx << " (chosen d3 value)";

				EXPECT_EQ(c.Rolls[1].Sides, 2) << "at index " << idx << " (chosen d2 sides)";
				EXPECT_EQ(c.Rolls[1].Value, chosenD2) << "at index " << idx << " (chosen d2 value)";
			}
		}
	}

	TEST_F(CombinationVisitorTest, AttackRollNormal_1d20_vs_10_Produces20CombinationsWith11SuccessesAnd9Failures)
	{
		auto attack = CreateAttackRollNode(CreateDice(1, 20), CreateConstant(10));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		attack->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();
		ASSERT_EQ(combinations.size(), 20u);

		size_t successes = 0;
		size_t failures = 0;

		// Expected: d20 roll values 1..20 paired with constant 10
		// Critical miss (1) => failure; values 2..9 => failure; 10..19 => success; 20 (crit) => success
		for (size_t i = 0; i < combinations.size(); ++i)
		{
			const auto& c = combinations[i];
			ASSERT_EQ(c.Rolls.size(), 1u);
			ASSERT_EQ(c.Rolls[0].Sides, 20);

			const int d20 = c.Rolls[0].Value;
			const int expected = (d20 == 1) ? 0 : ((d20 == 20) ? 1 : (d20 >= 10 ? 1 : 0));
			EXPECT_EQ(c.TotalValue, expected) << "at d20=" << d20;

			if (c.TotalValue == 1) ++successes; else ++failures;
		}

		EXPECT_EQ(successes, 11u);
		EXPECT_EQ(failures, 9u);
	}

	TEST_F(CombinationVisitorTest, AttackRollOnlyCritHitWillHit_1d20_vs_30_Produces20CombinationsWith1Success19Failures)
	{
		auto attack = CreateAttackRollNode(CreateDice(1, 20), CreateConstant(30));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		attack->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();
		ASSERT_EQ(combinations.size(), 20u);

		size_t successes = 0;
		size_t failures = 0;

		for (const auto& c : combinations)
		{
			ASSERT_EQ(c.Rolls.size(), 1u);
			ASSERT_EQ(c.Rolls[0].Sides, 20);

			const int d20 = c.Rolls[0].Value;
			const int expected = (d20 == 20) ? 1 : 0; // only crit hit
			EXPECT_EQ(c.TotalValue, expected) << "at d20=" << d20;

			if (c.TotalValue == 1) ++successes; else ++failures;
		}

		EXPECT_EQ(successes, 1u);
		EXPECT_EQ(failures, 19u);
	}

	TEST_F(CombinationVisitorTest, AttackRollOnlyCritMissWillMiss_1d20_vs_1_Produces20CombinationsWith19Successes1Failure)
	{
		auto attack = CreateAttackRollNode(CreateDice(1, 20), CreateConstant(1));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		attack->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();
		ASSERT_EQ(combinations.size(), 20u);

		size_t successes = 0;
		size_t failures = 0;

		for (const auto& c : combinations)
		{
			ASSERT_EQ(c.Rolls.size(), 1u);
			ASSERT_EQ(c.Rolls[0].Sides, 20);

			const int d20 = c.Rolls[0].Value;
			const int expected = (d20 == 1) ? 0 : 1; // crit miss fails, all others succeed
			EXPECT_EQ(c.TotalValue, expected) << "at d20=" << d20;

			if (c.TotalValue == 1) ++successes; else ++failures;
		}

		EXPECT_EQ(successes, 19u);
		EXPECT_EQ(failures, 1u);
	}

	TEST_F(CombinationVisitorTest, AttackRollLowAcWithAdvantage_ADV_1d20_vs_1_Produces400CombinationsWith399Successes1Failure)
	{
		// ADV over 2 attempts on 1d20 creates 20*20 combinations; armor class is constant 1
		auto attack = CreateAttackRollNode(CreateAdvantageNode(CreateDice(1, 20)), CreateConstant(1));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		attack->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();
		ASSERT_EQ(combinations.size(), 400u);

		size_t successes = 0;
		size_t failures = 0;

		for (const auto& c : combinations)
		{
			// Advantage combination records only the chosen attempt's single d20 roll
			ASSERT_EQ(c.Rolls.size(), 1u);
			ASSERT_EQ(c.Rolls[0].Sides, 20);

			const int chosenD20 = c.Rolls[0].Value;
			const int expected = (chosenD20 == 1) ? 0 : 1;
			EXPECT_EQ(c.TotalValue, expected);

			if (c.TotalValue == 1) ++successes; else ++failures;
		}

		EXPECT_EQ(successes, 399u);
		EXPECT_EQ(failures, 1u);
	}

	TEST_F(CombinationVisitorTest, AttackRollHighAcWithDisadvantage_DIS_1d20_vs_30_Produces400CombinationsWith1Success399Failures)
	{
		// DIS over 2 attempts on 1d20 creates 20*20 combinations; armor class is constant 30
		auto attack = CreateAttackRollNode(CreateDisadvantageNode(CreateDice(1, 20)), CreateConstant(30));

		DiceCalculator::Evaluation::CombinationAstVisitor visitor;
		attack->Accept(visitor);

		const auto& combinations = visitor.GetCombinations();
		ASSERT_EQ(combinations.size(), 400u);

		size_t successes = 0;
		size_t failures = 0;

		for (const auto& c : combinations)
		{
			// Disadvantage combination records only the chosen attempt's single d20 roll (the minimum)
			ASSERT_EQ(c.Rolls.size(), 1u);
			ASSERT_EQ(c.Rolls[0].Sides, 20);

			const int chosenD20 = c.Rolls[0].Value;
			const int expected = (chosenD20 == 20) ? 1 : 0; // only crit hit succeeds
			EXPECT_EQ(c.TotalValue, expected);

			if (c.TotalValue == 1) ++successes; else ++failures;
		}

		EXPECT_EQ(successes, 1u);
		EXPECT_EQ(failures, 399u);
	}
}