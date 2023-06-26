#include "FiberDataDisplayHandler.h"

#include "FiberData.h"
#include "FiberDataRenderOptionsWidget.h"
#include "FiberDataRenderer.h"

#include <ImFusion/Core/Assert.h>
#include <ImFusion/GUI/ImageView2D.h>
#include <ImFusion/GUI/ImageView3D.h>


namespace ImFusion
{
	bool FiberDataDisplayHandler::handlesType(const Data* data) const
	{
		return dynamic_cast<const FiberData*>(data) != nullptr;
	}


	bool FiberDataDisplayHandler::canBeShownInView(const Data* data, const InteractiveView& view) const
	{
		// show only in MPR and 3D views
		return (dynamic_cast<const ImageView2D*>(&view) != nullptr && view.view()->type() == GlView::SLICE3D) ||
			   (dynamic_cast<const ImageView3D*>(&view) != nullptr);
	}


	void FiberDataDisplayHandler::show(Data* data, InteractiveView& view)
	{
		IMFUSION_ASSERT(canBeShownInView(data, view));

		if (auto fiberData = dynamic_cast<FiberData*>(data))
		{
			auto it = m_displayInfos.find(fiberData);
			if (it == m_displayInfos.end())
			{
				// this is the first time we show this dataset -> create a new renderer instance
				it = m_displayInfos.insert({fiberData, DisplayInfo()}).first;
				it->second.m_renderer = std::make_unique<FiberDataRenderer>(*fiberData);
				fiberData->signalDeleted.connect(this, &FiberDataDisplayHandler::onDataDeleted);
			}

			if (it->second.views.count(&view) == 0)
			{
				// add the renderer to the view
				view.view()->addObject(it->second.m_renderer.get());
				it->second.views.insert(&view);
			}
		}
	}


	void FiberDataDisplayHandler::hide(Data* data, InteractiveView& view)
	{
		auto it = m_displayInfos.find(static_cast<FiberData*>(data));
		if (it == m_displayInfos.end())
			return;

		if (it->second.views.count(&view) > 0)
		{
			view.view()->removeObject(it->second.m_renderer.get());
			it->second.views.erase(&view);
		}

		// do *not* delete DisplayInfo here:
		// The user may toggle visibility several times and we want to avoid re-instantiating the FiberDataRenderer shaders over and over again.
		// The corresponding DisplayInfo instance is erased through the Data::signalDeleted callback.
	}


	std::unique_ptr<QWidget> FiberDataDisplayHandler::createDisplayOptionsWidget(Data* data, DisplayWidgetMulti& display, InteractiveView&) const
	{
		IMFUSION_ASSERT(handlesType(data));
		FiberData* sweep = static_cast<FiberData*>(data);
		auto it = m_displayInfos.find(sweep);
		if (it == m_displayInfos.end())
			return nullptr;

		// Create a Qt widget that is shown in the "Display Options" UI of the view and allows for configuring the renderer.
		return std::make_unique<FiberDataRenderOptionsWidget>(&data->components().getOrCreate<FiberDataRenderer::Options>(), &display);
	}


	void FiberDataDisplayHandler::onDataDeleted(const Data* data)
	{
		// Callback when a dataset has been deleted -> remove corresponding renderer
		auto it = m_displayInfos.find(static_cast<const FiberData*>(data));
		if (it == m_displayInfos.end())
		{
			IMFUSION_ASSERT(false, "should not reach this");
			return;
		}

		IMFUSION_ASSERT(it->second.views.empty(), "Fibers still shown in some views");
		m_displayInfos.erase(it);
	}


}
