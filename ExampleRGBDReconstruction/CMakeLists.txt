cmake_minimum_required(VERSION 3.13.0)
project(RGBDReconstruction)

# Locate the ImFusion SDK. List required modules/plugins in the COMPONENTS section.
find_package(ImFusionLib COMPONENTS ImFusionRGBD ImFusionVision REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Sources
    main.cpp
    MainWindow.cpp
    resources.qrc
)
set(Headers
    MainWindow.h
)

add_executable(RGBDReconstruction ${Sources} ${Headers})
target_include_directories(RGBDReconstruction PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RGBDReconstruction PRIVATE
    ImFusionLib
	ImFusionRGBD
	ImFusionStream
    Qt5::Widgets
)

imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()