# Define a new CMake project for the demo plugin
cmake_minimum_required(VERSION 3.8.0)
project(StreamExamples)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib COMPONENTS ImFusionStream REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
	DemoInputImageStream.cpp
    DemoStreamDataAlgorithm.cpp
    StreamExamplesFactory.cpp
    StreamExamplesPlugin.cpp
)
set(Headers
	DemoInputImageStream.h
	DemoStreamDataAlgorithm.h
	StreamExamplesFactory.h
	StreamExamplesPlugin.h
)
set(UiSources
)
set(Shaders
)

# Define target library
add_library(StreamExamples SHARED ${Sources} ${UiHeaders} ${Headers} ${Shaders})
target_include_directories(StreamExamples PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(StreamExamples PRIVATE
	ImFusionLib
	ImFusionStream
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

