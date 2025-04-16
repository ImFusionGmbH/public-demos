# Define a new CMake project for the demo application
cmake_minimum_required(VERSION 3.13.0)
project(DicomBrowser)

# Locate the ImFusion SDK. List required modules/plugins in the COMPONENTS section.
find_package(ImFusionLib COMPONENTS ImFusionDicom REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DicomBrowser.cpp
)
set(Headers
	DicomBrowser.h
)

# Define target executable
add_executable(DicomBrowser ${Sources} ${Headers})
target_include_directories(DicomBrowser PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the ImFusionLib and selected modules/plugins
target_link_libraries(DicomBrowser PRIVATE
	ImFusionLib
	ImFusionDicom
)

imfusion_set_common_target_properties()
