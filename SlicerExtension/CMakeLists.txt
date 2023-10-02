cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(ImFusionSlicerExtension)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "")
set(EXTENSION_CATEGORY "ImFusion")
set(EXTENSION_CONTRIBUTORS "")
set(EXTENSION_DESCRIPTION "")
set(EXTENSION_ICONURL "https://www.imfusion.com")
#set(EXTENSION_SCREENSHOTURLS "")
set(EXTENSION_DEPENDS "ImFusionLib") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(ImFusionAlgorithmExample)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
