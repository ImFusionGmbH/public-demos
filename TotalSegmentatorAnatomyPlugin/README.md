# Total Segmentator Demo Plugin

**Note:**
This demo requires the `ImFusion.TotalSegmentator` plugin, which is no longer part of the standard release of the ImFusion SDK. [Contact us](https://www.imfusion.com) for additional information.

## Summary
This tutorial explains how to build a simple custom plugin for the ImFusion SDK.
This plugin extends the ImFusionLib with a custom algorithm and a custom algorithm GUI. It performs the following operations:
1. Run TotalMeshSegmentator algorithm to label different input regions
2. Extract mesh from each segmented component
3. Store each mesh as a separate entity in AnatomicalStructureCollection (ASC) and label it using custom keys

![Screenshot of the Example Plugin in Action](screenshot.png)


## Requirements and Build Instructions
- Installed ImFusion SDK
- Qt5 (at least the version that the ImFusion SDK comes with)
- CMake version 3.2 or newer

Use CMake to generate build/project files for your build system of choice.
If you are using Visual Studio the CMake scripts will automatically configure the generated Solution with the correct environment parameters so that you can launch the ImFusion Suite including your plugin directly from Visual Studio.
If the ImFusion Suite does not pick up the built plugin on start, configure the `IMFUSION_PLUGIN_PATH` environment variable to contain .dll/.so directory.


## The Anatomy of an ImFusion Plugin

### The CMakeLists.txt
We use CMake as build system.
The [CMakeLists.txt](CMakeLists.txt) file defines the required input and configuration for building the demo plugin.
The first part contains the header section that defines the CMake project and locates 3rd-party libraries.
The second part defines the source files for the demo plugin and configures include directories as well as linker input.


### The TotalSegmentatorDemoPlugin class
Every ImFusion SDK plugin must define a custom plugin class inheriting from `ImFusion::ImFusionPlugin`.
This base class defines the interface so that the ImFusionLib and the plugin can interact with each other (e.g. add algorithm factories).
If you need to do any initialization work (e.g. register device code repositories) you should do this in the `TotalSegmentatorDemoPlugin` constructor.
See files [TotalSegmentatorDemoPlugin.h](TotalSegmentatorDemoPlugin.h) and [TotalSegmentatorDemoPlugin.cpp](TotalSegmentatorDemoPlugin.cpp)

**Note:** In addition to defining the TotalSegmentatorDemoPlugin class, you must register the Plugin via
```cpp
IMFUSION_REGISTER_PLUGIN(ImFusion::TotalSegmentatorDemoPlugin)
```


### The TotalSegmentatorDemoAlgorithmFactory and TotalSegmentatorDemoControllerFactory classes
As seen in the previous section the `TotalSegmentatorDemoPlugin` instantiates a `TotalSegmentatorDemoAlgorithmFactory` and a `TotalSegmentatorDemoControllerFactory`.
These factories allow to add custom algorithms and controllers to the ImFusion SDK.
They will for instance appear in the algorithm menu of the ImFusion Suite.
- [TotalSegmentatorDemoFactory.h](TotalSegmentatorDemoFactory.h) 
- [TotalSegmentatorDemoFactory.cpp](TotalSegmentatorDemoFactory.cpp)


### The TotalSegmentatorDemoAlgorithm and the corresponding controller
The `TotalSegmentatorDemoAlgorithm` class is a simple demonstration of extending the Algorithm interface.
On execution it will run the TotalSegmentator algorithm and generate a mesh for each segmented region.
Its controller is a generic class (`AlgorithmController`) provided by ImFusion SDK that can be used directly without any further customization.
- [TotalSegmentatorDemoAlgorithm.h](TotalSegmentatorDemoAlgorithm.h)
- [TotalSegmentatorDemoAlgorithm.cpp](TotalSegmentatorDemoAlgorithm.cpp)

The demo relies on `DefaultAlgorithmController` provided by `ImFusion` SDK to provide a simple GUI using Qt Widgets.

**Note:** Observe how both algorithm and controller are registered with the `DemoFactory` in [TotalSegmentatorDemoFactory.cpp](TotalSegmentatorDemoFactory.cpp).
