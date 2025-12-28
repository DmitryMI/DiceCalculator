set(AppName DiceCalculator.Gui)

cmake_policy(SET CMP0111 NEW)

find_package(PNG REQUIRED)
find_package(Qt6
    COMPONENTS
        Core
        Gui
        Widgets
)

find_package(spdlog REQUIRED)

qt_standard_project_setup()

set(AppSources
	"main.cpp" 
	"DiceCalculator/Ui/MainWindow.cpp"
    "DiceCalculator/Ui/Widgets/DiceExpressionInput.cpp"
    "DiceCalculator/Ui/Widgets/DiceExpressionBlock.cpp"
    "DiceCalculator/Controllers/ExpressionEvaluationController.cpp"
)

# qt_add_resources(AppSources "CityDraft/Resources.qrc")

qt_add_executable (
		${AppName}
		WIN32
		${AppSources}
	)

target_link_libraries(${AppName} PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
	spdlog::spdlog
	DiceCalculator
)

target_include_directories(${AppName} PRIVATE ./)

set_property(TARGET ${AppName} PROPERTY CXX_STANDARD 23)

qt_generate_deploy_app_script(
    TARGET ${AppName}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

if(MSVC)
	target_compile_options(${AppName} PUBLIC /EHa-)
endif()

install(SCRIPT ${deploy_script})