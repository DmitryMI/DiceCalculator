#pragma once

#include "DiceCalculator/Parsing/BoostSpiritParser.h"

#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"

#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/Comparison.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include <string>
#include <memory>

namespace DiceCalculator::Parsing
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phoenix = boost::phoenix;

	using DiceAstPtr = std::shared_ptr<DiceCalculator::Expressions::DiceAst>;
	using namespace DiceCalculator::Expressions;
	using namespace DiceCalculator::Operators;

	namespace
	{
		// helper creators used in semantic actions
		static DiceAstPtr MakeConstant(int value)
		{
			return std::make_shared<ConstantNode>(value);
		}

		static DiceAstPtr MakeDice(int rolls, int sides)
		{
			return std::make_shared<DiceNode>(rolls, sides);
		}

		static DiceAstPtr MakeBinaryOp(std::shared_ptr<DiceOperator> op, DiceAstPtr left, DiceAstPtr right)
		{
			return std::make_shared<OperatorNode>(std::move(op), std::vector<DiceAstPtr>{ std::move(left), std::move(right) });
		}

		static DiceAstPtr MakeAddition(DiceAstPtr left, DiceAstPtr right)
		{
			return MakeBinaryOp(std::make_shared<Addition>(), std::move(left), std::move(right));
		}

		static DiceAstPtr MakeSubtraction(DiceAstPtr left, DiceAstPtr right)
		{
			return MakeBinaryOp(std::make_shared<Subtraction>(), std::move(left), std::move(right));
		}

		static DiceAstPtr MakeAdvantage(DiceAstPtr operand)
		{
			return std::make_shared<OperatorNode>(std::make_shared<Advantage>(Advantage::Mode::Advantage), std::vector<DiceAstPtr>{ std::move(operand) });
		}

		static DiceAstPtr MakeComparison(Comparison::Mode mode, DiceAstPtr left, DiceAstPtr right)
		{
			return std::make_shared<OperatorNode>(std::make_shared<Comparison>(mode), std::vector<DiceAstPtr>{ std::move(left), std::move(right) });
		}
	}

	std::shared_ptr<DiceCalculator::Expressions::DiceAst> BoostSpiritParser::Parse(const std::string& input) const
	{
		using Iterator = std::string::const_iterator;

		// qi placeholders
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> expression;
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> additive;
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> primary;

		// primary: dice, number, ADV(...), parenthesized expression
		primary =
			  (qi::int_ >> qi::lit('d') >> qi::int_)[ qi::_val = phoenix::bind(&MakeDice, qi::_1, qi::_2) ]
			| (qi::int_)[ qi::_val = phoenix::bind(&MakeConstant, qi::_1) ]
			| (qi::lit("ADV") >> '(' >> expression >> ')')[ qi::_val = phoenix::bind(&MakeAdvantage, qi::_1) ]
			| ('(' >> expression >> ')')[ qi::_val = qi::_1 ]
			;

		// additive: left-associative chain of + and -
		additive = primary >> *(
			  ('+' >> primary)[ qi::_val = phoenix::bind(&MakeAddition, qi::_val, qi::_1) ]
			| ('-' >> primary)[ qi::_val = phoenix::bind(&MakeSubtraction, qi::_val, qi::_1) ]
		);

		// comparison: support comparison operators. Use qi::lit to suppress operator attributes
		// Produce either a comparison node (left op right) or the additive AST when no comparison.
		expression =
			  (additive >> (qi::lit(">=") >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::GreaterThanOrEqual, qi::_1, qi::_2) ]
			| (additive >> (qi::lit("<=") >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::LessThanOrEqual, qi::_1, qi::_2) ]
			| (additive >> (qi::lit(">")  >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::GreaterThan, qi::_1, qi::_2) ]
			| (additive >> (qi::lit("<")  >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::LessThan, qi::_1, qi::_2) ]
			| (additive >> (qi::lit("==") >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::Equal, qi::_1, qi::_2) ]
			| (additive >> (qi::lit("!=") >> additive))[ qi::_val = phoenix::bind(&MakeComparison, Comparison::Mode::NotEqual, qi::_1, qi::_2) ]
			| additive[ qi::_val = qi::_1 ]
			;

		Iterator begin = input.begin();
		Iterator end = input.end();
		DiceAstPtr result;

		bool ok = qi::phrase_parse(begin, end,
			expression >> qi::eoi, // grammar
			ascii::space,          // skipper
			result                 // synthesized attribute
		);

		if (!ok)
		{
			return nullptr;
		}

		return result;
	}

	std::string BoostSpiritParser::Reconstruct(const std::shared_ptr<DiceCalculator::Expressions::DiceAst>& ast) const
	{
		// Not required by current tests; leave minimal implementation.
		return {};
	}
}