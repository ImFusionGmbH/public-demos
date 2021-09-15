# Define a new CMake project for the MachineLearningInference plugin
project(MachineLearningInferencePlugin)
cmake_minimum_required(VERSION 3.2.0)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib COMPONENTS ImFusionML TorchPlugin REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Define and configure the CMake target
set(Sources
	RunPretrainedModelAlgorithm.cpp
	MachineLearningInferenceFactory.cpp
	MachineLearningInferencePlugin.cpp
)
set(Headers
	RunPretrainedModelAlgorithm.h	
	MachineLearningInferenceFactory.h
	MachineLearningInferencePlugin.h
)
set(UiSources	
)

# Define target library
add_library(MachineLearningInferencePlugin SHARED ${Sources} ${UiHeaders} ${Headers})
target_include_directories(MachineLearningInferencePlugin PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(MachineLearningInferencePlugin PRIVATE
	ImFusionLib
	ImFusionML # note that we do need to link against the ML plugin (but not against TorchPlugin or OnnxRuntimePlugin)
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config. 
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

