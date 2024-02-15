# Define a new CMake project for the demo application
project(DemoMainWindowBase)
cmake_minimum_required(VERSION 3.2.0)

# Locate the ImFusion SDK. List required modules/plugins in the COMPONENTS section.
find_package(ImFusionLib COMPONENTS ImFusionDicom REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DemoMainWindowBase.cpp
)
set(Headers
	DemoMainWindowBase.h
)

# Define target executable
add_executable(DemoMainWindowBase ${Sources} ${Headers})
target_include_directories(DemoMainWindowBase PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the ImFusionLib and selected modules/plugins
target_link_libraries(DemoMainWindowBase PRIVATE
	ImFusionLib
	ImFusionDicom
)

imfusion_set_common_target_properties()
