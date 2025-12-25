set(
	DiceCalculator.Sources
	"DiceCalculator/Logging/LogManager.cpp"
	"DiceCalculator/Expressions/DiceNode.cpp"
	"DiceCalculator/Expressions/ConstantNode.cpp"
	"DiceCalculator/Expressions/OperatorNode.cpp"
	"DiceCalculator/Evaluation/DistributionAstVisitor.cpp"
	"DiceCalculator/Evaluation/RollAstVisitor.cpp"
	"DiceCalculator/Operators/Addition.cpp"
	"DiceCalculator/Operators/Subtraction.cpp"
	"DiceCalculator/Operators/Advantage.cpp"
	"DiceCalculator/StdRandom.cpp"
)

add_library(DiceCalculator ${DiceCalculator.Sources})
target_include_directories(DiceCalculator PUBLIC "../public")
set_property(TARGET DiceCalculator PROPERTY CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Boost REQUIRED COMPONENTS signals2 url assert geometry math serialization)
# target_link_libraries(DiceCalculator PUBLIC Boost::signals2 Boost::url Boost::assert Boost::geometry Boost::math Boost::serialization)

find_package(spdlog REQUIRED)
target_link_libraries(DiceCalculator PUBLIC spdlog::spdlog)

if(MSVC)
	target_compile_options(DiceCalculator PUBLIC /EHa-)
endif()
