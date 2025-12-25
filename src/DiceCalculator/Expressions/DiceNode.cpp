#pragma once

#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Evaluation/DiceAstVisitor.h"

namespace DiceCalculator::Expressions
{
	void DiceNode::Accept(Evaluation::DiceAstVisitor& v) const { v.Visit(*this); }
}