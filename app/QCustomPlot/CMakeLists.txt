set(QCustomPlotLibName QCustomPlot)

# Find Qt6 components required by QCustomPlot
find_package(Qt6
    REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
)

# Define the static library
add_library(${QCustomPlotLibName} STATIC
    qcustomplot.cpp
    qcustomplot.h
)

# Set C++ standard
set_property(TARGET ${QCustomPlotLibName} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${QCustomPlotLibName} PROPERTY CXX_STANDARD_REQUIRED ON)

# Link Qt libraries
target_link_libraries(${QCustomPlotLibName}
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
)

# Set include directories
target_include_directories(${QCustomPlotLibName}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Enable Qt MOC (Meta-Object Compiler) for QCustomPlot
set_target_properties(${QCustomPlotLibName} PROPERTIES
    AUTOMOC ON
)
