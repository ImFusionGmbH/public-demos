cmake_minimum_required(VERSION 3.13.0)
project(DicomExtensionDemoPlugin)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib COMPONENTS ImFusionDicom REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DemoDicomExtensionFactory.cpp
    DemoDicomExtensionPlugin.cpp
)
set(Headers
	DemoDicomExtensionFactory.h
	DemoDicomExtensionPlugin.h
    DemoDataComponent.h
    DemoExtension.h
)
set(UiSources)

# Define target library
add_library(DicomExtensionDemoPlugin SHARED ${Sources} ${UiHeaders} ${Headers})
target_include_directories(DicomExtensionDemoPlugin PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(DicomExtensionDemoPlugin PRIVATE
	ImFusionLib
	ImFusionDicom
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config.
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()
