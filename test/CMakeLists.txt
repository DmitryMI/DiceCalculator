cmake_minimum_required(VERSION 3.20)

project(DiceCalculator.Test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DiceCalculator.Test.Sources 
	"DiceCalculator/Expressions/DiceAstTest.cpp"
	"DiceCalculator/Evaluation/RollAstVisitorTest.cpp"
	"DiceCalculator/DistributionTest.cpp"
)

enable_testing()

add_executable(DiceCalculator.Test ${DiceCalculator.Test.Sources} )
target_link_libraries(DiceCalculator.Test PRIVATE DiceCalculator)
target_include_directories(DiceCalculator.Test PRIVATE "./")

find_package(spdlog REQUIRED)
target_link_libraries(DiceCalculator.Test PRIVATE spdlog::spdlog)

find_package(gtest REQUIRED)
target_link_libraries(DiceCalculator.Test PRIVATE GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(DiceCalculator.Test)

if(MSVC)
	target_compile_options(DiceCalculator.Test PUBLIC /EHa)
endif()