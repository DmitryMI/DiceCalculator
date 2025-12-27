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
		// ASSERT_TRUE(DiceCalculator::Operators::Advantage::registered);

		BoostSpiritParser parser;
		const std::string input = "1d6";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		auto expected = CreateDice(1, 6);
		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionOuterAdvantage)
	{
		BoostSpiritParser parser;
		const std::string input = "ADV(1d6)";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		auto expected = CreateAdvantageNode(CreateDice(1, 6));
		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionAttackRoll)
	{
		BoostSpiritParser parser;
		const std::string input = "AttackRoll(1d20, 13)";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		auto expected = CreateAttackRollNode(CreateDice(1, 20), CreateConstant(13));
		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionWithConstantAndAdvantage)
	{
		BoostSpiritParser parser;
		const std::string input = "ADV(2d10+4)-1d6";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		// Expected: ADV(2d10 + 4) + 1d6
		auto innerAdd = CreateAdditionNode({ CreateDice(2, 10), CreateConstant(4) });
		auto adv = CreateAdvantageNode(innerAdd);
		auto expected = CreateSubtractionNode({ adv, CreateDice(1, 6) });

		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionWithConstantAndAdvantageReordered)
	{
		BoostSpiritParser parser;
		const std::string input = "1d6+ADV(2d10+4)";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		// Expected: ADV(2d10 + 4) + 1d6
		auto innerAdd = CreateAdditionNode({ CreateDice(2, 10), CreateConstant(4) });
		auto adv = CreateAdvantageNode(innerAdd);
		auto expected = CreateAdditionNode({ CreateDice(1, 6), adv });

		EXPECT_TRUE(ast->IsEqual(*expected)) << "Parsed AST did not match expected for input: " << input;
	}

	TEST_F(BoostSpiritParserTest, ParseExpressionWithTwoAdditions)
	{
		BoostSpiritParser parser;
		const std::string input = "2d10+4+1d6";
		auto ast = parser.Parse(input);
		ASSERT_NE(ast, nullptr) << "Parser returned null for input: " << input;

		// Expected: ADV(2d10 + 4) + 1d6
		auto innerAdd = CreateAdditionNode({ CreateDice(2, 10), CreateConstant(4) });
		auto outerAdd = CreateAdditionNode({ innerAdd,  CreateDice(1, 6) });

		EXPECT_TRUE(ast->IsEqual(*outerAdd)) << "Parsed AST did not match expected for input: " << input;
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

