cmake_minimum_required(VERSION 3.13.0)
project(ImageMathDemo)

# Locate the ImFusion SDK.
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib COMPONENTS ImFusionImageMath REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
    DemoImageMathAlgorithm.cpp
    DemoImageMathFactory.cpp
    DemoImageMathPlugin.cpp
)
set(Headers
	DemoImageMathAlgorithm.h
	DemoImageMathFactory.h
	DemoImageMathPlugin.h
)
set(UiSources
)
set(Shaders
)

# Define target library
add_library(ImageMathDemo SHARED ${Sources} ${UiHeaders} ${Headers} ${Shaders})
target_include_directories(ImageMathDemo PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Enable OpenMP
if(APPLE)
	execute_process(
		COMMAND brew --prefix libomp
		OUTPUT_VARIABLE libomp_prefix
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${libomp_prefix}/include" CACHE STRING "")
	set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${libomp_prefix}/include" CACHE STRING "")
	set(OpenMP_CXX_LIB_NAMES "omp" CACHE STRING "")
	set(OpenMP_C_LIB_NAMES "omp" CACHE STRING "")
	set(OpenMP_omp_LIBRARY "${libomp_prefix}/lib/libomp.dylib" CACHE PATH "")
endif()

find_package(OpenMP QUIET)
if(OPENMP_FOUND)
	if(APPLE)
		target_link_libraries(ImageMathDemo PRIVATE OpenMP::OpenMP_CXX)
	else()
		target_compile_options(ImageMathDemo PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${OpenMP_CXX_FLAGS}>)
		target_link_libraries(ImageMathDemo PRIVATE ${OpenMP_CXX_FLAGS})
	endif()
endif()

# Link agains the ImFusionLib and selected modules/plugins
target_link_libraries(ImageMathDemo PRIVATE
	ImFusionLib
	ImFusionImageMath
)

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config.
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

