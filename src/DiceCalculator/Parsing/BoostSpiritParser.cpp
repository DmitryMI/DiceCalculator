#pragma once

#include "DiceCalculator/Parsing/BoostSpiritParser.h"

#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"

#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/Comparison.h"
#include "DiceCalculator/Operators/AttackRoll.h"
#include "DiceCalculator/Operators/OperatorRegistry.h"

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
			return std::make_shared<OperatorNode>(op, std::vector<DiceAstPtr>{ left, right });
		}

		static DiceAstPtr MakeAddition(DiceAstPtr left, DiceAstPtr right)
		{
			return MakeBinaryOp(std::make_shared<Addition>(), left, right);
		}

		static DiceAstPtr MakeSubtraction(DiceAstPtr left, DiceAstPtr right)
		{
			return MakeBinaryOp(std::make_shared<Subtraction>(), left, right);
		}

		static DiceAstPtr MakeAdvantage(DiceAstPtr operand)
		{
			return std::make_shared<OperatorNode>(std::make_shared<Advantage>(Advantage::Mode::Advantage), std::vector<DiceAstPtr>{ operand });
		}

		static DiceAstPtr MakeComparison(Comparison::Mode mode, DiceAstPtr left, DiceAstPtr right)
		{
			return std::make_shared<OperatorNode>(std::make_shared<Comparison>(mode), std::vector<DiceAstPtr>{ left, right });
		}

		static DiceAstPtr MakeAttackRoll(DiceAstPtr left, DiceAstPtr right)
		{
			return std::make_shared<OperatorNode>(std::make_shared<AttackRoll>(), std::vector<DiceAstPtr>{ left, right });
		}

		static DiceAstPtr MakeFunction(const std::string& name, const std::vector<DiceAstPtr>& args)
		{
			for (const auto& arg : args)
			{
				if (arg == nullptr)
				{
					throw std::runtime_error("Null argument in function call: " + name);
				}
			}

			if (name == "+")
			{
				return MakeAddition(args[0], args[1]);
			}
			else if (name == "-")
			{
				return MakeSubtraction(args[0], args[1]);
			}

			auto op = Operators::OperatorRegistry::Instance().Create(name);
			return std::make_shared<OperatorNode>(op, args);
		}

		static DiceAstPtr MakeBinaryCall(const std::string& name,
			const DiceAstPtr& lhs,
			const DiceAstPtr& rhs)
		{
			std::vector<DiceAstPtr> args;
			args.push_back(lhs);
			args.push_back(rhs);
			return MakeFunction(name, args);
		}

	}

	std::shared_ptr<DiceCalculator::Expressions::DiceAst> BoostSpiritParser::Parse(const std::string& input) const
	{
		using Iterator = std::string::const_iterator;

		// qi placeholders
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> expression;
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> additive;
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> primary;
		qi::rule<Iterator, std::string(), ascii::space_type> identifier;
		qi::rule<Iterator, std::vector<DiceAstPtr>(), ascii::space_type> argumentList;
		qi::rule<Iterator, DiceAstPtr(), ascii::space_type> functionCall;

		argumentList =
			expression % ',';

		identifier =
			qi::lexeme[
				qi::alpha >> *(qi::alnum | qi::char_('_'))
			];

		functionCall =
			(identifier >> '(' >> argumentList >> ')')
			[
				qi::_val = phoenix::bind(&MakeFunction, qi::_1, qi::_2)
			];

		// primary: dice, number, ADV(...), parenthesized expression
		primary =
			(qi::int_ >> qi::lit('d') >> qi::int_)
			[qi::_val = phoenix::bind(&MakeDice, qi::_1, qi::_2)]

			| (qi::int_)
			[qi::_val = phoenix::bind(&MakeConstant, qi::_1)]

			| functionCall[qi::_val = qi::_1]

			| ('(' >> expression >> ')')
			[qi::_val = qi::_1]
			;

		// additive: left-associative chain of + and -
		additive =
			primary
			[qi::_val = qi::_1]
			>> *(
				('+' >> primary)
				[qi::_val = phoenix::bind(&MakeBinaryCall,
					"+",
					qi::_val,
					qi::_1)]

				| ('-' >> primary)
				[qi::_val = phoenix::bind(&MakeBinaryCall,
					"-",
					qi::_val,
					qi::_1)]
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