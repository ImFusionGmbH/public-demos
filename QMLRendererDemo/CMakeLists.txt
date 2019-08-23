project(QMLRendererDemo)
cmake_minimum_required(VERSION 3.10.0)

find_package(Qt5 COMPONENTS Core Quick Gui Widgets OpenGL REQUIRED)
find_package(ImFusionLib REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# define and configure the target
set(Sources
	ImFusionClient.cpp	
	ImFusionFboView.cpp	
	MainWindow.cpp	
	main.cpp
)

set(Headers
	ImFusionClient.h
	ImFusionFboView.h
	MainWindow.h
)

set(UiSources
	main.qml
)

set(ResourceFiles 
	standalone.qrc
)


add_executable(QMLRendererDemo ${Sources} ${Headers} ${UiSources} ${ResourceFiles})
target_link_libraries(QMLRendererDemo
	PUBLIC
		ImFusionLib
	PRIVATE
		Qt5::Core
		Qt5::Quick
		Qt5::Gui
)
target_compile_definitions(QMLRendererDemo PRIVATE "IMFUSIONLIB_DIR=${ImFusionLib_DIR}")

imfusion_set_common_target_properties()
