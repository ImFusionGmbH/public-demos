# Define a new CMake project for the demo plugin
project(Example2D3DRegistrationPlugin)
cmake_minimum_required(VERSION 3.2.0)


# Locate the ImFusion SDK. We require the CT component to
# use the XRay2D3DRegistrationAlgorithm class.
find_package(ImFusionLib REQUIRED COMPONENTS ImFusionCT)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
  Example2D3DRegistrationAlgorithm.cpp
  Example2D3DRegistrationController.cpp
  Example2D3DRegistrationFactory.cpp
  Example2D3DRegistrationPlugin.cpp
  Custom2D3DRegistrationInitialization.cpp
  )
set(Headers
  Example2D3DRegistrationAlgorithm.h
  Example2D3DRegistrationController.h
  Example2D3DRegistrationFactory.h
  Example2D3DRegistrationPlugin.h
  Custom2D3DRegistrationInitialization.h
  )

# Define target library
add_library(Example2D3DRegistrationPlugin SHARED ${Sources} ${UiHeaders} ${Headers})
target_include_directories(Example2D3DRegistrationPlugin PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
# Link against the ImFusionLib and selected modules/plugins
target_link_libraries(Example2D3DRegistrationPlugin PRIVATE
  ImFusionLib
  ImFusionCT
  )

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

