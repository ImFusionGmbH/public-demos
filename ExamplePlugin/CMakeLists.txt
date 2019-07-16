# Define a new CMake project for the demo plugin
project(DemoPlugin)
cmake_minimum_required(VERSION 3.2.0)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DemoAlgorithm.cpp
    DemoController.cpp
    DemoFactory.cpp
    DemoPlugin.cpp
)
set(Headers
	DemoAlgorithm.h
	DemoController.h
	DemoFactory.h
	DemoPlugin.h
)
set(UiSources
	DemoController.ui
)

# Define target library
add_library(DemoPlugin SHARED ${Sources} ${UiHeaders} ${Headers})
target_include_directories(DemoPlugin PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(DemoPlugin PRIVATE
	ImFusionLib
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

