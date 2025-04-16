#pragma once

#include "FiberData.h"
#include "FiberDataRenderer.h"

#include <ImFusion/Core/Mat.h>
#include <ImFusion/Core/Signal.h>
#include <ImFusion/GUI/DataDisplayHandler.h>

#include <set>
#include <unordered_map>

namespace ImFusion
{
	/// Implements the DataDisplayHandler interface so that FiberData can be shown in views.
	class FiberDataDisplayHandler : public DataDisplayHandler, public SignalReceiver
	{
	public:
		bool handlesType(const Data* data) const override;
		bool canBeShownInView(const Data* data, const GUI::View& view) const override;
		void show(Data* data, GUI::View& view) override;
		void hide(Data* data, GUI::View& view) override;
		std::unique_ptr<QWidget> createDisplayOptionsWidget(Data* data, GUI::DisplayBase& display, GUI::View& view) const override;

	private:
		void onDataDeleted(const Data* data);

		struct DisplayInfo
		{
			std::set<GUI::View*> views;                       ///< Keeps track in which views the FiberData instance is currently shown
			std::unique_ptr<FiberDataRenderer> m_renderer;    ///< The actual renderer instance
		};
		std::unordered_map<const FiberData*, DisplayInfo> m_displayInfos;
	};
}
