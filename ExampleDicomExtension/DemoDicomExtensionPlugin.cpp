#include "DemoDicomExtensionPlugin.h"

#include "DemoDataComponent.h"
#include "DemoDicomExtensionFactory.h"
#include "DemoExtension.h"

#include <ImFusion/Base/DataComponentFactory.h>
#include <ImFusion/Dicom/DicomIOD_Registry.h>

#include <memory>

// This macro creates the entry point allowing the ImFusion SDK to discover
// and load this plugin at runtime.
IMFUSION_REGISTER_PLUGIN(ImFusion::DemoDicomExtensionPlugin)

namespace ImFusion
{
	DemoDicomExtensionPlugin::DemoDicomExtensionPlugin() = default;

	DemoDicomExtensionPlugin::~DemoDicomExtensionPlugin() = default;

	PluginBase::Status DemoDicomExtensionPlugin::init()
	{
		// Register DataComponent
		DataComponentFactory::get().registerComponent(DemoDataComponent().id(), []() { return std::make_unique<DemoDataComponent>(); });
		// Register DICOM extension
		Dicom::IOD_Registry::registerDefaultExtension(std::make_unique<DemoExtension>());

		registerFactories(std::make_unique<DemoDicomExtensionAlgorithmFactory>(), std::make_unique<DemoDicomExtensionControllerFactory>(), nullptr);

		return Status::Success;
	}


}
