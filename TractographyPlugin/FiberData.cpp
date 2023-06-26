#include "FiberData.h"

#include <limits>

namespace ImFusion
{
	FiberData::FiberData()
		: m_boundsDirty(false)
		, m_minAnisotropy(std::numeric_limits<float>::quiet_NaN())
		, m_maxAnisotropy(std::numeric_limits<float>::quiet_NaN())
	{
	}


	void FiberData::addFiber(const std::vector<vec4f>& vertices)
	{
		auto begin = vertices.begin();
		auto end = vertices.end();
		const size_t numElems = vertices.size();
		if (numElems == 0)
			return;

		const auto fiberId = m_fibers.size();
		size_t startIndex = m_vertices.size();
		m_vertices.insert(m_vertices.end(), begin, end);
		size_t endIndex = m_vertices.size();

		float length = 0.f;
		vec4f lastCoord = *begin;
		vec3f dirPrev = vec3f::Zero();
		vec3f dirNext = vec3f::Zero();
		vec3f tangentSum = vec3f::Zero();
		for (auto it = begin; it != end; ++it)
		{
			if (it + 1 != end)
				dirNext = (it + 1)->head<3>() - it->head<3>();
			m_tangents.push_back(vec4f::Zero());
			m_tangents.back() << (dirPrev + dirNext).normalized(), float(fiberId);
			tangentSum += m_tangents.back().head<3>();
			dirPrev = dirNext;

			length += (*it - lastCoord).norm();
			lastCoord = *it;

			m_minAnisotropy = std::min((*it)[3], m_minAnisotropy);
			m_maxAnisotropy = std::max((*it)[3], m_maxAnisotropy);
		}

		const vec3f avgTangent = tangentSum / float(numElems);
		m_fibers.emplace_back(startIndex, endIndex, avgTangent, length);
		m_boundsDirty = true;
		signalFibersChanged.emitSignal();
	}


	int FiberData::size() const
	{
		return static_cast<int>(m_fibers.size());
	}


	void FiberData::clear()
	{
		m_fibers.clear();
		m_vertices.clear();
		m_tangents.clear();
		m_minAnisotropy = std::numeric_limits<float>::quiet_NaN();
		m_maxAnisotropy = std::numeric_limits<float>::quiet_NaN();
		m_bounds = Bounds();
		m_matrix = mat4::Identity();
		signalFibersChanged.emitSignal();
	}


	const std::vector<FiberData::Fiber>& FiberData::fibers() const
	{
		return m_fibers;
	}


	const std::vector<vec4f>& FiberData::vertices() const
	{
		return m_vertices;
	}


	const std::vector<ImFusion::vec4f>& FiberData::tangents() const
	{
		return m_tangents;
	}


	Data::Kind FiberData::kind() const
	{
		return Data::UNKNOWN;
	}


	Pose::TransformationConvention FiberData::matrixConvention() const
	{
		return Pose::TOWORLD;
	}


	Bounds FiberData::bounds() const
	{
		if (m_boundsDirty)
		{
			m_bounds = Bounds();
			for (auto& v : m_vertices)
			{
				// a vertex is a vec4, however the fourth coordinate is not the homogeneous coordinate but the anisotropy value.
				// Therefore, we need to do the `head<3>().homogeneous()`.
				m_bounds.addPoint((m_matrix * v.cast<double>().head<3>().homogeneous()).head<3>());
			}
			m_boundsDirty = false;
		}

		return m_bounds;
	}


	void FiberData::setMatrix(const mat4& m)
	{
		Data::setMatrix(m);
		m_boundsDirty = true;
	}


	std::string FiberData::describe() const
	{
		return "Fiber Tract with " + std::to_string(m_fibers.size()) + " fibers and " + std::to_string(m_vertices.size()) + " vertices.";
	}


}
