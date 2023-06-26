#pragma once
#include <ImFusion/Base/IoAlgorithm.h>
#include <ImFusion/Core/Filesystem/Path.h>
#include <ImFusion/Core/Mat.h>
#include <ImFusion/Core/Parameter.h>

#include <memory>

namespace ImFusion
{
	class FiberData;

	/// Very basic IO algorithm to read/write TrackVis .trk files into/from FiberData.
	/// \see http://trackvis.org/docs/?subsect=fileformat
	class FiberDataIoAlgorithm : public IoAlgorithm
	{
	public:
		explicit FiberDataIoAlgorithm(const FiberData* input = nullptr);
		~FiberDataIoAlgorithm();

		static bool createCompatible(const DataList& data, Algorithm** a = nullptr);
		virtual void compute() override;
		virtual OwningDataList takeOutput() override;
		virtual std::vector<std::string> supportedFileExtensions() const override;

		Parameter<vec3f> p_scaling = {"scaling", vec3f(1, 1, 1), this};    ///< Optional scaling of tracts applied on load
		Parameter<vec3f> p_offset = {"offset", vec3f(0, 0, 0), this};      ///< Optional offset of tracts applied on load

	private:
		std::unique_ptr<FiberData> readTrkFile(const Filesystem::Path& filename) const;
		bool saveTrkFile(const FiberData& fibers, const Filesystem::Path& filename) const;

		const FiberData* m_input;
		std::unique_ptr<FiberData> m_output;
	};
}
