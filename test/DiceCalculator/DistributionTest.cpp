#include <gtest/gtest.h>
#include "DiceCalculator/Distribution.h"
#include "DiceCalculator/Combination.h"

namespace DiceCalculator::Expressions
{
	TEST(DistributionTest, InitListInsertAccumulateAndOrder)
	{
		// Use initializer-list constructor with duplicate key 3 to test merging
		Distribution d = { {3, 0.2}, {1, 0.5}, {3, 0.3} }; // 3 -> 0.5, 1 -> 0.5

		EXPECT_EQ(d.Size(), 2u);

		const auto& data = d.GetData();
		ASSERT_EQ(data.size(), 2u);
		EXPECT_EQ(data[0].first, 1);
		EXPECT_DOUBLE_EQ(data[0].second, 0.5);
		EXPECT_EQ(data[1].first, 3);
		EXPECT_DOUBLE_EQ(data[1].second, 0.5);
	}

	TEST(DistributionTest, AssignmentFromInitList)
	{
		Distribution d;
		d = { {4, 0.7}, {2, 0.3} };

		EXPECT_EQ(d.Size(), 2u);
		EXPECT_DOUBLE_EQ(d[2], 0.3);
		EXPECT_DOUBLE_EQ(d[4], 0.7);
	}

	TEST(DistributionTest, OperatorIndexBehavior)
	{
		Distribution d;
		d[5] = 0.25; // inserts key 5
		EXPECT_EQ(d.Size(), 1u);
		EXPECT_DOUBLE_EQ(d[5], 0.25);

		// const access should not insert a new key
		const Distribution& cd = d;
		EXPECT_DOUBLE_EQ(cd[5], 0.25);
		EXPECT_DOUBLE_EQ(cd[4], 0.0); // 4 is missing, const operator[] returns 0.0
		EXPECT_EQ(d.Size(), 1u);
	}

	TEST(DistributionTest, Normalize)
	{
		Distribution d = { {1, 2.0}, {2, 3.0} };

		d.Normalize();

		const auto& data = d.GetData();
		ASSERT_EQ(data.size(), 2u);
		EXPECT_NEAR(data[0].second + data[1].second, 1.0, 1e-12);
		EXPECT_NEAR(data[0].second, 2.0 / 5.0, 1e-12);
		EXPECT_NEAR(data[1].second, 3.0 / 5.0, 1e-12);
	}

	TEST(DistributionTest, EraseWhenProbabilityZero)
	{
		Distribution d = { {1, 0.5}, {1, -0.5} }; // cancels to 0 -> erased

		EXPECT_EQ(d.Size(), 0u);
	}

	TEST(DistributionTest, ClearAndReserve)
	{
		Distribution d;
		d.Reserve(10);
		d.AddOutcome(2, 0.1);
		d.AddOutcome(3, 0.2);

		EXPECT_EQ(d.Size(), 2u);

		d.Clear();
		EXPECT_EQ(d.Size(), 0u);
	}

	TEST(DistributionTest, FromCombinationsAggregatesTotalsAndNormalizes)
	{
		// Prepare combinations: totals [4, 5, 5, 7]
		// Rolls content is irrelevant for distribution; include some to ensure it's ignored.
		std::vector<Combination> combos;
		combos.push_back(Combination{ 4, { Combination::Roll{ 6, 4 } } });
		combos.push_back(Combination{ 5, { Combination::Roll{ 6, 5 } } });
		combos.push_back(Combination{ 5, { Combination::Roll{ 3, 2 }, Combination::Roll{ 3, 3 } } });
		combos.push_back(Combination{ 7, { Combination::Roll{ 6, 4 }, Combination::Roll{ 6, 3 } } });

		Distribution d = Distribution::FromCombinations(combos);

		// Expect values 4,5,7 with probabilities 1/4, 2/4, 1/4 respectively.
		ASSERT_EQ(d.Size(), 3u);

		const auto& data = d.GetData();
		ASSERT_EQ(data.size(), 3u);
		EXPECT_EQ(data[0].first, 4);
		EXPECT_NEAR(data[0].second, 1.0 / 4.0, 1e-12);

		EXPECT_EQ(data[1].first, 5);
		EXPECT_NEAR(data[1].second, 2.0 / 4.0, 1e-12);

		EXPECT_EQ(data[2].first, 7);
		EXPECT_NEAR(data[2].second, 1.0 / 4.0, 1e-12);

		// Sum to 1
		EXPECT_NEAR(data[0].second + data[1].second + data[2].second, 1.0, 1e-12);
	}
}