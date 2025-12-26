#include <gtest/gtest.h>

#include "DiceCalculator/Parsing/BoostSpiritParser.h"
#include "DiceCalculator/TestUtilities.h"

#include <string>

namespace DiceCalculator::Parsing
{
	class BoostSpiritParserTest : public DiceCalculator::TestUtilities::TestHelpers
	{
	};

	TEST_F(BoostSpiritParserTest, ParseSimpleDice)
	{
		BoostSpiritParser parser;
		const std::string input = "1d6";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		auto expected = CreateDice(1, 6);
		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionWithConstantAndAdvantage)
	{
		BoostSpiritParser parser;
		const std::string input = "ADV(2d10+4)+1d6";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		// Expected: ADV(2d10 + 4) + 1d6
		auto innerAdd = CreateAdditionNode({ CreateDice(2, 10), CreateConstant(4) });
		auto adv = CreateAdvantageNode(innerAdd);
		auto expected = CreateAdditionNode({ adv, CreateDice(1, 6) });

		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseComplexNestedComparison)
	{
		BoostSpiritParser parser;
		const std::string input = "(1d8 + (2d4 - 3)) >= 5";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		// Expected: (1d8 + (2d4 - 3)) >= 5
		auto sub = CreateSubtractionNode({ CreateDice(2, 4), CreateConstant(3) });
		auto leftAdd = CreateAdditionNode({ CreateDice(1, 8), sub });
		auto expected = CreateGreaterThanOrEqualNode(leftAdd, CreateConstant(5));

		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}
}

