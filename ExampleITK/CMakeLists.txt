# [CMake Header]
# Define a new CMake project for the ITK plugin
project(ITKPlugin)
cmake_minimum_required(VERSION 3.2.0)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
if(NOT TARGET ImFusionLib)
	find_package(ImFusionLib REQUIRED)
endif()

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Locate ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# [Define and configure the CMake target]
set(Sources
    ITKCannyEdgeAlgorithm.cpp
    ITKFactory.cpp
    ITKPlugin.cpp
	ITK.cpp
)
set(Headers
	ITKCannyEdgeAlgorithm.h
	ITKFactory.h
	ITKPlugin.h
	ITK.h
)

# Define target library
add_library(ITKPlugin SHARED ${Sources} ${Headers})
target_include_directories(ITKPlugin PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}			# needed for the UI files
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(ITKPlugin PRIVATE
	ImFusionLib
	${ITK_LIBRARIES}
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()
