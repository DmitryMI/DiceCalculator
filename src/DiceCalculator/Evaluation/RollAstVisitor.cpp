#pragma once

#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"

namespace DiceCalculator::Evaluation
{
	void RollAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		m_Result = node.GetValue();
	}

	void RollAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		int total = 0;
		for(int i = 0; i < node.GetRolls(); ++i)
		{
			total += m_Random.NextInt(1, node.GetSides());
		}
		m_Result = total;
	}

	void RollAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		m_Result = node.GetOperator()->Roll(*this, node.GetOperands());
	}
}