#pragma once

#include <ImFusion/Base/DataComponent.h>
#include <ImFusion/Core/GL/GlFwd.h>
#include <ImFusion/Core/Mat.h>
#include <ImFusion/Core/Parameter.h>
#include <ImFusion/Core/Signal.h>
#include <ImFusion/GL/GlObject.h>

#include <deque>
#include <vector>

namespace ImFusion
{
	class FiberData;

	/// Renderer for FiberData instances.
	///
	/// This class implements the GlObject interface so that it can be rendered in a GlView.
	/// Rendering is performed using GLSL shaders and buffers.
	class FiberDataRenderer : public GlObject, public SignalReceiver
	{
	public:
		/// Helper record to configure the appearance of FiberData rendered by the FiberDataRenderer.
		///
		/// This class implements the DataComponent interface so that we can easily attach it to a FiberData
		/// instance to store the configuration together with the parent dataset.
		class Options : public DataComponent<FiberDataRenderer::Options>
		{
		public:
			enum class RenderMode
			{
				Stripes,    ///< Render as shaded stripes
				Tubes       ///< Render as tubes
			};

			enum class ColoringMode
			{
				LocalTangent,
				AverageTangent,
				CenterTangent,
				ProjectedTangentHSV
			};

			bool operator==(const Options& rhs) const;
			std::string id() const override;


			Parameter<RenderMode> p_renderMode = {"renderMode", RenderMode::Tubes, this};
			Parameter<ColoringMode> p_coloringMode = {"coloringMode", ColoringMode::AverageTangent, this};

			/// Diameter of the rendered fibers (mm)
			Parameter<double> p_fiberWidth = {"fiberWidth", 0.25, this};

			/// Minimum length of a fiber to be displayed.
			Parameter<double> p_lengthThreshold = {"lengthThreshold", 0.0, this};

			/// Flag whether to color-code local anisotropy values using saturation.
			Parameter<bool> p_enableAnisotropyColorcoding = {"enableAnisotropyColorcoding", true, this};
		};


		/// Instantiates a new renderer for displaying *fiberData*.
		/// You can optionally provide a set of custom *renderOptions* to use. Otherwise, it will use
		/// the render options in `fiberData.components()` by default.
		explicit FiberDataRenderer(const FiberData& fiberData, const Options* renderOptions = nullptr);
		~FiberDataRenderer();

		virtual void draw(const GlView& view) override;
		virtual std::string typeName() const override;
		virtual Bounds bounds() const override;


	private:
		void updateBuffers();

		const FiberData& m_fiberData;      ///< Fibers to render
		const Options* m_renderOptions;    ///< Render options to use

		std::unique_ptr<GL::Program> m_prog;                 ///< GLSL shader used for rendering
		std::unique_ptr<GL::VertexBuffer> m_vertexBuffer;    ///< OpenGL buffer storing per-vertex position and tangent
		std::unique_ptr<GL::ShaderStorageBuffer> m_ssbo;     ///< OpenGL buffer storing per-fiber metadata
		std::vector<size_t> m_restartIndices;                ///< Indices in m_vertexBuffer when each individual fiber starts

		bool m_buffersDirty = true;
	};
}
