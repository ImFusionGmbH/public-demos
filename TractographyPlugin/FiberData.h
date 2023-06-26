#pragma once

#include <ImFusion/Base/Data.h>
#include <ImFusion/Base/DataComponent.h>
#include <ImFusion/Core/Mat.h>
#include <ImFusion/Core/Signal.h>

#include <deque>
#include <numeric>
#include <vector>

namespace ImFusion
{
	/// Custom data type to store fiber tracts reconstructed from DTI imaging.
	///
	/// This class implements the ImFusion::Data interface so that it can be stored in the DataModel,
	/// serve as input/output of algorithms, and displayed in views.
	/// An instance of this class is formed of a set of fiber tracts, each defined by a series of vertices.
	/// The actual implementation is kept very minimalistic
	class FiberData : public Data
	{
	public:
		/// Structure storing meta information of a single fiber
		struct Fiber
		{
			size_t startIndex;    ///< Start index of the fiber within the FiberData object
			size_t endIndex;      ///< End index of the fiber (as in STL iterators: points to the element *behind* the last vertex)
			vec3f avgTangent;     ///< average tangent direction
			float length;         ///< Length of the fiber

			Fiber(size_t startIndex, size_t endIndex, const vec3f& avgTangent, float length)
				: startIndex(startIndex)
				, endIndex(endIndex)
				, avgTangent(avgTangent)
				, length(length){};
		};


		FiberData();

		/// Adds a new fiber defined by *vertices*.
		/// The first 3 components of each vertex define its coordinate, 
		/// the fourth component defines an optional scalar anisotropy value.
		void addFiber(const std::vector<vec4f>& vertices);

		int size() const;
		void clear();

		const std::vector<Fiber>& fibers() const;
		const std::vector<vec4f>& vertices() const;
		const std::vector<vec4f>& tangents() const;

		float minAnisotropy() const { return m_minAnisotropy; }
		float maxAnisotropy() const { return m_maxAnisotropy; }

		virtual Kind kind() const override;
		virtual Pose::TransformationConvention matrixConvention() const override;
		virtual Bounds bounds() const override;
		virtual void setMatrix(const mat4& m) override;
		std::string describe() const override;


		Signal<> signalFibersChanged;


	private:
		std::vector<Fiber> m_fibers;
		std::vector<vec4f> m_vertices;
		std::vector<vec4f> m_tangents;
		float m_minAnisotropy;
		float m_maxAnisotropy;

		mutable Bounds m_bounds;
		mutable bool m_boundsDirty;
	};

}
