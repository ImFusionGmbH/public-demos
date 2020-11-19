# Define a new CMake project for the OpenCV plugin
project(ExampleOpenCV)
cmake_minimum_required(VERSION 3.2.0)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib REQUIRED)

# Locate OpenCV. We are using core and imgproc for this example
find_package(OpenCV REQUIRED COMPONENTS core imgproc)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    CannyEdgeAlgorithm.cpp
    OpenCVFactory.cpp
    OpenCVPlugin.cpp
)
set(Headers
	CannyEdgeAlgorithm.h
	OpenCVFactory.h
	OpenCVPlugin.h
)


# Define target library
add_library(ExampleOpenCV SHARED ${Sources} ${UiHeaders} ${Headers})
target_include_directories(ExampleOpenCV PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(ExampleOpenCV PRIVATE
	ImFusionLib
)

# Link against OpenCV libraries
target_link_libraries(ExampleOpenCV PRIVATE ${OpenCV_LIBS})

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()