project(TractographyPlugin)
cmake_minimum_required(VERSION 3.13.0)

# Locate the ImFusion SDK, .
# List required modules/plugins in the COMPONENTS section (e.g. COMPONENTS ImFusionSeg ImFusionReg).
find_package(ImFusionLib REQUIRED)

# Enable automatic MOC, RCC and UIC preprocessing for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Define and configure the CMake target
set(Sources
	FiberDataRenderer.cpp
	FiberData.cpp
	FiberDataDisplayHandler.cpp
	FiberDataRenderOptionsWidget.cpp
	FiberDataIoAlgorithm.cpp
	TractographyPlugin.cpp
)
set(Headers
	FiberData.h
	FiberDataDisplayHandler.h
	FiberDataRenderOptionsWidget.h
	FiberDataIoAlgorithm.h
	FiberDataRenderer.h
	TractographyPlugin.h
)
set(Shaders
	shaders/FiberDataRenderer.frag
	shaders/FiberDataRenderer.geom
	shaders/FiberDataRenderer.vert
)
set(Resources
	Resources.qrc
)

# Define target library
add_library(TractographyPlugin SHARED ${Sources} ${Headers} ${Shaders} ${Resources})
target_include_directories(TractographyPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against the ImFusionLib, this plugin does not require any additional plugins
target_link_libraries(TractographyPlugin PRIVATE ImFusionLib)

# Create a resource respository containing the GLSL shaders so that they can be loaded at runtime
imfusion_compile_resource_repository(
	TractographyPluginShaders
	FILES ${Shaders}
	BASE_DIR "shaders"
)


if(MSVC)
	target_compile_options(TractographyPlugin PRIVATE
		"/MP"		# Enable parallel compilation
		"/wd4251"	# disable warnings: class 'type' needs to have dll-interface to be used by clients of class 'type2'
	)
endif()

# Define output target directories and provide instructions on how to launch
# the ImFusion Suite with the built custom plugin.
# These functions are provided by the ImFusionLib target config.
imfusion_set_common_target_properties()
imfusion_provide_ide_instructions()

