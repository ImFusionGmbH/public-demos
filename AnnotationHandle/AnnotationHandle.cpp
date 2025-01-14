#include "AnnotationHandle.h"

#include <ImFusion/Base/Math.h>
#include <ImFusion/Core/GL/FixedFunctionPipeline.h>
#include <ImFusion/GL/GlPointBasedAnnotation.h>
#include <ImFusion/GL/GlSliceView.h>
#include <ImFusion/GL/GlVolumeView.h>
#include <ImFusion/GUI/ImageView2D.h>
#include <ImFusion/GUI/InteractiveView.h>

namespace ImFusion
{
	namespace
	{
		constexpr double HandleWidth = 20.0;
		constexpr double HandleHeight = 20.0;
		constexpr double HandleXCoord = 0.0;
		constexpr double HandleYCoord = 40.0;
	}

	GlAnnotationHandle::GlAnnotationHandle(GlPointBasedAnnotation* pba)
		: GlObject()
		, m_pba(pba)
	{
	}

	void GlAnnotationHandle::draw(const GlView& view)
	{
		// Don't draw the handle if the referenced GlPointBasedAnnotation is not visible
		if (!m_pba || !m_pba->visible() || !visible())
			return;

		// In triangleVertices for each annotation point we store the upward, right, down and left pointing arrows/triangles.
		std::vector<vec3> triangleVertices;
		triangleVertices.reserve(12 * m_pba->points().size());

		for (const auto& point : m_pba->points())
		{
			// The triangles are supposed to have a fixed size in pixels, however we render in world coordinates around point. This gives us the relationship between the two coordinate spaces.
			auto [x, y, z] = view.state().unprojectPixelIncrement(point);

			// up arrow
			triangleVertices.insert(triangleVertices.end(),
									{point + HandleXCoord * x + (HandleYCoord + (HandleHeight / 2)) * y,
									 point + (HandleXCoord + (HandleWidth / 2)) * x + (HandleYCoord - (HandleHeight / 2)) * y,
									 point + (HandleXCoord - (HandleWidth / 2)) * x + (HandleYCoord - (HandleHeight / 2)) * y});
			// down arrow
			triangleVertices.insert(triangleVertices.end(),
									{point - HandleXCoord * x - (HandleYCoord + (HandleHeight / 2)) * y,
									 point - (HandleXCoord + (HandleWidth / 2)) * x - (HandleYCoord - (HandleHeight / 2)) * y,
									 point - (HandleXCoord - (HandleWidth / 2)) * x - (HandleYCoord - (HandleHeight / 2)) * y});
			// left arrow
			triangleVertices.insert(triangleVertices.end(),
									{point - HandleXCoord * y - (HandleYCoord + (HandleWidth / 2)) * x,
									 point - (HandleXCoord + (HandleHeight / 2)) * y - (HandleYCoord - (HandleWidth / 2)) * x,
									 point - (HandleXCoord - (HandleHeight / 2)) * y - (HandleYCoord - (HandleWidth / 2)) * x});
			// right arrow
			triangleVertices.insert(triangleVertices.end(),
									{point + HandleXCoord * y + (HandleYCoord + (HandleWidth / 2)) * x,
									 point + (HandleXCoord + (HandleHeight / 2)) * y + (HandleYCoord - (HandleWidth / 2)) * x,
									 point + (HandleXCoord - (HandleHeight / 2)) * y + (HandleYCoord - (HandleWidth / 2)) * x});
		}

		if (!triangleVertices.empty())
		{
			GL::FixedFunctionPipeline& ffp = GL::FixedFunctionPipeline::cachedInstance();
			ffp.enable(view.state(), GL::FixedFunctionPipeline::ShaderSelection::Default, view.orderIndependentTransparency());
			// Set line color and width
			ffp.setDefaultColor(vec4(1.0, 0.5, 0.0, 1.0));
			GL::FixedFunctionPipeline::makeVertexData(triangleVertices).assign(m_vbo);
			m_vbo.draw(GL::Primitive::Triangles);
			ffp.disable();
		}
	}

	InteractiveAnnotationHandle::InteractiveAnnotationHandle(GlPointBasedAnnotation* pba, const int& stepSize)
		: InteractiveObject(std::make_unique<GlAnnotationHandle>(pba).release())
		, m_pba(pba)
		, m_stepSize(stepSize)
	{
	}

	namespace
	{
		enum class MoveDirection
		{
			Up,
			Down,
			Left,
			Right
		};

		std::optional<MoveDirection> clickedDirection(const GlView& view, const QMouseEvent& mouseEvent, const vec3& point)
		{
			const auto pointPosProj = view.state().project(point);

			// We need to check if the handle is actually visible at the position clicked (might be invisible due to clipping or overdraw).
			// The logic is dependent on the type of the view.
			if (dynamic_cast<const GlSliceView*>(&view))
			{
				// Rendered objects are in z value range 0 to 1 for GlSliceView
				if (abs(pointPosProj.z() - 0.5) > 0.5)
					return std::nullopt;
			}
			else if (const auto glVolumeView = dynamic_cast<const GlVolumeView*>(&view))
			{
				// For GlVolumeView we use the first hit point location's z value of the position clicked and check if it is roughly the same as the annotation point's.
				const GL::Viewport& viewport = glVolumeView->state().viewport();
				int x = mouseEvent.x() - viewport.x;
				int y = mouseEvent.y() - viewport.y;
				if (const auto fhp = glVolumeView->firstHitpointLocation({x, y}))
				{
					if (const auto fhpProj = view.state().project(*fhp); abs(pointPosProj.z() - fhpProj.z()) > 0.0005)
						return std::nullopt;
				}
				else
				{
					return std::nullopt;
				}
			}
			else
			{
				// We do not consider other view types.
				return std::nullopt;
			}

			// Now that we know a potentially clicked button is rendered we check which was actually clicked.
			const auto xDist = pointPosProj.x() - mouseEvent.x();
			const auto xDistAbs = std::abs(xDist);
			const auto yDist = pointPosProj.y() - mouseEvent.y();
			const auto yDistAbs = std::abs(yDist);

			if (xDistAbs < HandleWidth / 2)
			{
				if (yDistAbs > HandleYCoord - HandleHeight / 2 && yDistAbs < HandleYCoord + HandleHeight / 2)
				{
					if (yDist > 0)
						return MoveDirection::Down;
					else
						return MoveDirection::Up;
				}
			}
			else if (yDistAbs < HandleHeight / 2)
			{
				if (xDistAbs > HandleYCoord - HandleWidth / 2 && xDistAbs < HandleYCoord + HandleWidth / 2)
				{
					if (xDist > 0)
						return MoveDirection::Left;
					else
						return MoveDirection::Right;
				}
			}
			return std::nullopt;
		}

		void moveAnnotationPoint(const GlView& view, const MoveDirection& direction, vec3& point, const int stepSize)
		{
			vec3 step;
			switch (direction)
			{
			case MoveDirection::Up:
				step = vec3(0, stepSize, 0);
				break;
			case MoveDirection::Down:
				step = vec3(0, -stepSize, 0);
				break;
			case MoveDirection::Right:
				step = vec3(stepSize, 0, 0);
				break;
			case MoveDirection::Left:
				step = vec3(-stepSize, 0, 0);
				break;
			}
			auto pointPosProj = view.state().project(point);
			pointPosProj += step;
			point = view.state().unproject(pointPosProj);
		}
	}

	bool InteractiveAnnotationHandle::sceneEvent(QEvent* event, InteractiveView* view)
	{
		// Only consider left mouse button press events
		if (const auto* currentMouseEvent = dynamic_cast<QMouseEvent*>(event);
			currentMouseEvent && currentMouseEvent->type() == QEvent::MouseButtonPress && currentMouseEvent->button() == Qt::LeftButton)
		{
			// Only make it interactive if the handle and its referenced GlPointBasedAnnotation are visible
			if (m_pba->visible() && view && view->view())
			{
				const GlView& glView = *view->view();
				// Get all the points in the GlPointBasedAnnotation
				std::vector<vec3> points = m_pba->points();
				for (auto& point : points)
				{
					// Check for all the points of the GlPointBasedAnnotation if any of their arrows was clicked
					if (const auto direction = clickedDirection(glView, *currentMouseEvent, point))
					{
						// If an arrow was clicked move its point in the appropriate direction
						moveAnnotationPoint(glView, direction.value(), point, m_stepSize);
						// Update the points in the GlPointBasedAnnotation
						m_pba->setPoints(points);
						// Return true to signal that the event was consumed
						return true;
					}
				}
			}
		}
		// Return false to signal that the event was not consumed
		return false;
	}

}
