#include "FiberDataRenderer.h"

#include "FiberData.h"

#include <ImFusion/Core/GL/Program.h>
#include <ImFusion/Core/GL/ShaderStorageBuffer.h>
#include <ImFusion/Core/GL/StateGuard.h>
#include <ImFusion/Core/GL/VertexBuffer.h>
#include <ImFusion/GL/Camera.h>


namespace ImFusion
{
	bool FiberDataRenderer::Options::operator==(const FiberDataRenderer::Options& rhs) const
	{
		return p_renderMode == rhs.p_renderMode && p_coloringMode == rhs.p_coloringMode && p_fiberWidth == rhs.p_fiberWidth &&
			   p_lengthThreshold == rhs.p_lengthThreshold && p_enableAnisotropyColorcoding == rhs.p_enableAnisotropyColorcoding;
	}


	std::string FiberDataRenderer::Options::id() const
	{
		return "TractographyPlugin.FiberDataRenderer.Options";
	}


	FiberDataRenderer::FiberDataRenderer(const FiberData& fiberData, const Options* renderOptions)
		: m_fiberData(fiberData)
		, m_renderOptions(renderOptions ? renderOptions : &const_cast<FiberData&>(fiberData).components().getOrCreate<Options>())
	{
		// need to update OpenGL buffers every time the FiberData instancec has changed
		m_fiberData.signalFibersChanged.connect(this, [this]() { m_buffersDirty = true; });
	}


	FiberDataRenderer::~FiberDataRenderer() = default;


	void FiberDataRenderer::draw(const GlView& view)
	{
		if (!m_visible || m_fiberData.size() == 0)
			return;

		IMFUSION_ASSERT(m_renderOptions != nullptr);
		updateBuffers();

		const mat4& viewMatrix = view.state().modelViewMatrix();
		Camera camera = Camera::fromPose(viewMatrix, Camera::OpenGL);

		m_prog->setDefines(m_renderOptions->p_renderMode == Options::RenderMode::Stripes ? "#define DO_STRIPES\n" : "#define DO_TUBES\n");

		const mat4 modelMatrix = m_fiberData.matrixToWorld();
		const mat3f normalMatrix = modelMatrix.inverse().transpose().topLeftCorner<3, 3>().cast<float>();

		m_prog->enable();
		m_prog->setArgument("u_projectionMatrix", view.state().projectionMatrix());
		m_prog->setArgument("u_viewMatrix", view.state().modelViewMatrix());
		m_prog->setArgument("u_modelMatrix", modelMatrix);
		m_prog->setArgument("u_normalMatrix", normalMatrix);
		m_prog->setArgument("u_cameraPosition", camera.position());
		m_prog->setArgument("u_lookVector", camera.lookVector().normalized().eval());
		m_prog->setArgument("u_fiberWidth", m_renderOptions->p_fiberWidth.value());
		m_prog->setArgument("u_lengthThreshold", m_renderOptions->p_lengthThreshold.value());
		m_prog->setArgument("u_minAnisotropy", m_renderOptions->p_enableAnisotropyColorcoding.value() ? m_fiberData.minAnisotropy() : 0.f);
		m_prog->setArgument("u_maxAnisotropy", m_renderOptions->p_enableAnisotropyColorcoding.value() ? m_fiberData.maxAnisotropy() : 0.f);
		m_prog->setArgument("u_coloringMode", int(m_renderOptions->p_coloringMode.value()));
		m_prog->bindBuffer("u_metaData", *m_ssbo);

		m_vertexBuffer->drawMultiRestart(GL::Primitive::LineStrip, m_restartIndices);

		m_prog->disable();
	}


	std::string FiberDataRenderer::typeName() const
	{
		return "FiberDataRenderer";
	}


	Bounds FiberDataRenderer::bounds() const
	{
		return m_fiberData.bounds();
	}


	void FiberDataRenderer::updateBuffers()
	{
		if (!m_buffersDirty)
			return;

		if (!m_prog)
			m_prog = GL::Program::createVertexGeometryFragment("TractographyPluginShaders/FiberDataRenderer.vert",
															   "TractographyPluginShaders/FiberDataRenderer.geom",
															   "TractographyPluginShaders/FiberDataRenderer.frag",
															   "#define DO_TUBES\n");
		if (!m_vertexBuffer)
			m_vertexBuffer = std::make_unique<GL::VertexBuffer>();
		if (!m_ssbo)
			m_ssbo = std::make_unique<GL::ShaderStorageBuffer>();

		const auto& vertices = m_fiberData.vertices();
		const auto& tangents = m_fiberData.tangents();
		m_restartIndices.clear();
		m_restartIndices.reserve(m_fiberData.fibers().size());

		struct FiberMetaData
		{
			vec3f avgDirection;
			float length;
			vec3f centerDirection;
			float padding;
		};
		std::vector<FiberMetaData> metaData(m_fiberData.fibers().size(), FiberMetaData{});

		size_t f = 0;
		for (auto& fiber : m_fiberData.fibers())
		{
			metaData[f].avgDirection = fiber.avgTangent;
			metaData[f].length = fiber.length;
			metaData[f].centerDirection = tangents[fiber.startIndex + (fiber.endIndex - fiber.startIndex) / 2].head<3>();

			m_restartIndices.push_back(fiber.startIndex);
			++f;
		}

		m_ssbo->bind();
		m_ssbo->setData(metaData);
		m_ssbo->unbind();

		m_vertexBuffer->bind();
		m_vertexBuffer->setData(vertices, tangents);
		m_vertexBuffer->unbind();
		m_buffersDirty = false;
	}
}
