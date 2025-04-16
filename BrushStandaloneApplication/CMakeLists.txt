# Define a new CMake project for the brush application
cmake_minimum_required(VERSION 3.13.0)
project(BrushApplication)

# Locate the ImFusion SDK. List required modules/plugins in the COMPONENTS section.
find_package(ImFusionLib COMPONENTS ImFusionDicom ImFusionSeg REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
	BrushApplication.cpp
)
set(Headers
	BrushApplication.h
)

# Define target executable
add_executable(BrushApplication ${Sources} ${Headers})
target_include_directories(BrushApplication PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(BrushApplication PRIVATE cxx_std_20)

# Link against the ImFusionLib and selected modules/plugins
target_link_libraries(BrushApplication PRIVATE
	ImFusionLib
	ImFusionDicom
	ImFusionSeg
)

imfusion_set_common_target_properties()
