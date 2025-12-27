#pragma once

#include "DiceCalculator/Evaluation/CombinationAstVisitor.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include <stdexcept>

namespace DiceCalculator::Evaluation
{
	void CombinationAstVisitor::Visit(const Expressions::ConstantNode& node)
	{
		throw std::runtime_error("Not Implemented.");
	}

	void CombinationAstVisitor::Visit(const Expressions::DiceNode& node)
	{
		throw std::runtime_error("Not Implemented.");
	}

	void CombinationAstVisitor::Visit(const Expressions::OperatorNode& node)
	{
		throw std::runtime_error("Not Implemented.");
	}
}