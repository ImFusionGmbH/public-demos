# Define a new CMake project for the demo plugin
cmake_minimum_required(VERSION 3.13.0)
project(OpenGLDemo)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DemoOpenGLAlgorithm.cpp
    DemoOpenGLFactory.cpp
    DemoOpenGLPlugin.cpp
    DemoOpenGLController.cpp
    MyCustomGlObject.cpp
)
set(Headers
	DemoOpenGLAlgorithm.h
	DemoOpenGLFactory.h
	DemoOpenGLPlugin.h
    DemoOpenGLController.h
    MyCustomGlObject.h
)
set(UiSources
)
set(Shaders
	myShader.frag
)

# Define target library
add_library(OpenGLDemo SHARED ${Sources} ${UiHeaders} ${Headers} ${Shaders})
target_include_directories(OpenGLDemo PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(OpenGLDemo PRIVATE
	ImFusionLib
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config.
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

