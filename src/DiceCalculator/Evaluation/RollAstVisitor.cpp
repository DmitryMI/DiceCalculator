#pragma once

#include "DiceCalculator/Evaluation/RollAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"

namespace DiceCalculator::Evaluation
{
	void RollAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		m_DiceRecords.clear();
		m_Result = node.GetValue();
	}

	void RollAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		m_DiceRecords.clear();
		int total = 0;
		for(int i = 0; i < node.GetRolls(); ++i)
		{
			int rolledValue = m_Random.NextInt(1, node.GetSides());
			total += rolledValue;
			m_DiceRecords.push_back({ node.GetSides(), rolledValue });
		}
		m_Result = total;
	}

	void RollAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		m_DiceRecords.clear();
		m_Result = node.GetOperator()->Evaluate(*this, node.GetOperands());
	}
}