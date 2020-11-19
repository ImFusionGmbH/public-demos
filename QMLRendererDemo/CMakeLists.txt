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


if(MSVC)
	# install a post-build step to deploy Qt stuff: https://doc.qt.io/qt-5/windows-deployment.html
	option(IMFUSION_RUN_WINDEPLOYQT "Install Qt dependencies in binary dir" ON)
	
	get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
	find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
	set(WINDEPLOYQT_ARGS --qmldir "${CMAKE_CURRENT_SOURCE_DIR}" --no-translations --no-system-d3d-compiler --no-compiler-runtime --pdb)
	add_custom_command(TARGET QMLRendererDemo POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E
		env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:QMLRendererDemo>" ${WINDEPLOYQT_ARGS}
		COMMENT "Running windeployqt..."
	)
endif()