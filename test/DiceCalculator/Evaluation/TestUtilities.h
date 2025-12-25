#pragma once

#include <gtest/gtest.h>

#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/IRandom.h"

#include <memory>
#include <vector>

namespace DiceCalculator::TestUtilities
{
	// Simple deterministic IRandom for tests (extracted from RollAstVisitorTest)
	class MockRandom : public DiceCalculator::IRandom
	{
	public:
		explicit MockRandom(std::vector<int> values) : m_Values(std::move(values)), m_Index(0) {}

		int NextInt(int /*minInclusive*/, int /*maxInclusive*/) override
		{
			if (m_Index >= m_Values.size())
				return 0;
			return m_Values[m_Index++];
		}

	private:
		std::vector<int> m_Values;
		std::size_t m_Index;
	};

	// Helper fixture providing node construction utilities shared across tests.
	class TestHelpers : public ::testing::Test
	{
	protected:
		std::shared_ptr<DiceCalculator::Expressions::ConstantNode> CreateConstant(int v) const
		{
			return std::make_shared<DiceCalculator::Expressions::ConstantNode>(v);
		}

		std::shared_ptr<DiceCalculator::Expressions::DiceNode> CreateDice(int rolls, int sides) const
		{
			return std::make_shared<DiceCalculator::Expressions::DiceNode>(rolls, sides);
		}

		std::shared_ptr<DiceCalculator::Expressions::OperatorNode> CreateAdditionNode(std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>> operands) const
		{
			return std::make_shared<DiceCalculator::Expressions::OperatorNode>(std::make_shared<DiceCalculator::Operators::Addition>(), std::move(operands));
		}

		std::shared_ptr<DiceCalculator::Expressions::OperatorNode> CreateAdvantageNode(std::shared_ptr<DiceCalculator::Expressions::DiceAst> operand) const
		{
			return std::make_shared<DiceCalculator::Expressions::OperatorNode>(std::make_shared<DiceCalculator::Operators::Advantage>(DiceCalculator::Operators::Advantage::Mode::Advantage), std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>>{ operand });
		}

		std::shared_ptr<DiceCalculator::Expressions::OperatorNode> CreateDisadvantageNode(std::shared_ptr<DiceCalculator::Expressions::DiceAst> operand) const
		{
			return std::make_shared<DiceCalculator::Expressions::OperatorNode>(std::make_shared<DiceCalculator::Operators::Advantage>(DiceCalculator::Operators::Advantage::Mode::Disadvantage), std::vector<std::shared_ptr<DiceCalculator::Expressions::DiceAst>>{ operand });
		}
	};
}