set(
	DiceCalculator.Sources
	"DiceCalculator/Logging/LogManager.cpp"
)

add_library(DiceCalculator ${DiceCalculator.Sources})
target_include_directories(DiceCalculator PUBLIC "../public")
set_property(TARGET DiceCalculator PROPERTY CXX_STANDARD 23)

# find_package(Boost REQUIRED COMPONENTS signals2 url assert geometry math serialization)
# target_link_libraries(DiceCalculator PUBLIC Boost::signals2 Boost::url Boost::assert Boost::geometry Boost::math Boost::serialization)

find_package(spdlog REQUIRED)
target_link_libraries(DiceCalculator PUBLIC spdlog::spdlog)

# find_package(Stb REQUIRED)
# target_include_directories(DiceCalculator PUBLIC ${Stb_INCLUDE_DIR})

if(MSVC)
	target_compile_options(DiceCalculator PUBLIC /EHa-)
endif()
