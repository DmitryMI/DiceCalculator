set(
	DiceCalculator.Sources
	"DiceCalculator/Evaluation/DistributionAstVisitor.cpp"
	"DiceCalculator/Evaluation/RollAstVisitor.cpp"
	"DiceCalculator/Expressions/ConstantNode.cpp"
	"DiceCalculator/Expressions/DiceNode.cpp"
	"DiceCalculator/Expressions/OperatorNode.cpp"
	"DiceCalculator/Logging/LogManager.cpp"
	"DiceCalculator/Operators/Addition.cpp"
	"DiceCalculator/Operators/Advantage.cpp"
	"DiceCalculator/Operators/AttackRoll.cpp"
	"DiceCalculator/Operators/Comparison.cpp"
	"DiceCalculator/Operators/Subtraction.cpp"
	"DiceCalculator/Parsing/BoostSpiritParser.cpp"
	"DiceCalculator/StdRandom.cpp"
)

add_library(DiceCalculator ${DiceCalculator.Sources})
target_include_directories(DiceCalculator PUBLIC "../public")
set_property(TARGET DiceCalculator PROPERTY CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS Spirit)
target_link_libraries(DiceCalculator PUBLIC Boost::spirit)

find_package(spdlog REQUIRED)
target_link_libraries(DiceCalculator PUBLIC spdlog::spdlog)

# Create a reusable interface target and link it to targets that need the flag.
add_library(compiler_flags INTERFACE)
target_compile_options(compiler_flags INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

# then add to your library/targets:
target_link_libraries(DiceCalculator PUBLIC compiler_flags)
# repeat for other targets as needed
