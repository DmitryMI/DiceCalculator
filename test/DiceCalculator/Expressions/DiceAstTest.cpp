#include <gtest/gtest.h>
#include "DiceCalculator/Expressions/DiceAst.h"
#include "DiceCalculator/Expressions/OperatorNode.h"
#include "DiceCalculator/Expressions/DiceNode.h"
#include "DiceCalculator/Expressions/ConstantNode.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/Addition.h"

namespace DiceCalculator::Expressions
{
    TEST(DiceAstTest, ConstructTest)
    {
        std::shared_ptr<DiceAst> ast =
            std::make_shared<OperatorNode>(std::make_shared<Operators::Advantage>(Operators::Advantage::Mode::Advantage),
                std::vector<std::shared_ptr<DiceAst>>{
                    std::make_shared<OperatorNode>(std::make_shared<Operators::Addition>(), std::vector<std::shared_ptr<DiceAst>>{
                        std::make_unique<DiceNode>(2, 10),
                        std::make_unique<DiceNode>(4, 20),
                        std::make_unique<ConstantNode>(10)
            })
        });
    }


}