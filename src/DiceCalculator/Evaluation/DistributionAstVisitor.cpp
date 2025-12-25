#pragma once

#include "DiceCalculator/Evaluation/DistributionAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"

namespace DiceCalculator::Evaluation
{
	void DistributionAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		m_Distribution = { {node.GetValue(), 1.0} };
	}

	void DistributionAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		// TODO: Implement dice distribution calculation
	}

	void DistributionAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		std::vector<Distribution> args;
		for (auto& op : node.GetOperands())
		{
			op->Accept(*this);
			args.push_back(m_Distribution);
		}
		// TODO: Implement operator distribution calculation based on node.GetSymbol()
	}
}