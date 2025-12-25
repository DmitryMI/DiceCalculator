#include <gtest/gtest.h>
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"

namespace DiceCalculator::Expressions
{
	TEST(DiceAstTest, ConstructTest)
	{
        std::shared_ptr<DiceAst> ast =
            std::make_shared<OperatorNode>("ADV", std::vector<std::shared_ptr<DiceAst>>{
            std::make_shared<OperatorNode>("+", std::vector<std::shared_ptr<DiceAst>>{
                std::make_unique<DiceNode>(2, 10),
                    std::make_unique<DiceNode>(4, 20),
                    std::make_unique<ConstantNode>(10)
            })
        });

	}

	
}