#include "FiberDataIoAlgorithm.h"

#include "FiberData.h"

#include <ImFusion/Base/Math.h>
#include <ImFusion/Base/SharedImageSet.h>
#include <ImFusion/Base/TypedImage.h>
#include <ImFusion/Base/Utils/Images.h>
#include <ImFusion/Core/Log.h>
#include <ImFusion/Core/Platform.h>

#include <cstring>
#include <deque>
#include <fstream>
#include <string.h>


namespace
{
	// clang-format off
	// File header for TrackVis *.trk files
	// http://trackvis.org/docs/?subsect=fileformat
	struct trk_header {
		//                                   SIZE  DESCRIPTION
		char id_string[6];                   //6   ID string for track file. The first 5 characters must be "TRACK".
		short int dim[3];                    //6   Dimension of the image volume.
		float voxel_size[3];                 //12  Voxel size of the image volume.
		float origin[3];                     //12  Origin of the image volume. This field is not yet being used by TrackVis. That means the origin is always (0, 0, 0).
		short int n_scalars;                 //2   Number of scalars saved at each track point (besides x, y and z coordinates).
		char scalar_name[10][20];            //200 Name of each scalar. Can not be longer than 20 characters each. Can only store up to 10 names.
		short int n_properties;              //2   Number of properties saved at each track.
		char property_name[10][20];          //200 Name of each property. Can not be longer than 20 characters each. Can only store up to 10 names.
		float vox_to_ras[4][4];              //64  4x4 matrix for voxel to RAS (crs to xyz) transformation.
											 //    If vox_to_ras[3][3] is 0, it means the matrix is not recorded.
											 //    This field is added from version 2.
		char reserved[444];                  //444 Reserved space for future version.
		char voxel_order[4];                 //4   Storing order of the original image data. Explained here.
		char pad2[4];                        //4   Paddings.
		float image_orientation_patient[6];  //24  Image orientation of the original image. As defined in the DICOM header.
		char pad1[2];                        //2   Paddings.
		unsigned char invert_x;              //1   Inversion/rotation flags used to generate this track file. For internal use only.
		unsigned char invert_y;              //1   As above.
		unsigned char invert_z;              //1   As above.
		unsigned char swap_xy;               //1   As above.
		unsigned char swap_yz;               //1   As above.
		unsigned char swap_zx;               //1   As above.
		int32_t n_count;                     //4   Number of tracks stored in this track file. 0 means the number was NOT stored.
		int32_t version;                     //4   Version number. Current version is 2.
		int32_t hdr_size;                    //4   Size of the header. Used to determine byte swap. Should be 1000.
	};
	// clang-format on
}

namespace ImFusion
{

	FiberDataIoAlgorithm::FiberDataIoAlgorithm(const FiberData* input)
		: IoAlgorithm(input != nullptr ? IoMode::Write : IoMode::Read, LocationType::File)
		, m_input(input)
	{
		if (m_ioMode == IoMode::Read)
		{
			// setting the `useroption` attribute makes it appear in the automatically generated GUI
			p_scaling.setAttribute("useroption", "r");
			p_offset.setAttribute("useroption", "r");
		}
	}


	FiberDataIoAlgorithm::~FiberDataIoAlgorithm() = default;


	bool FiberDataIoAlgorithm::createCompatible(const DataList& data, Algorithm** a /*= 0*/)
	{
		if (data.size() == 0)
		{
			if (a)
				*a = new FiberDataIoAlgorithm(nullptr);
			return true;
		}

		auto fibers = data.getAll<FiberData>();
		if (fibers.size() == 1 && static_cast<int>(fibers.size()) == data.size())
		{
			if (a)
				*a = new FiberDataIoAlgorithm(fibers[0]);
			return true;
		}

		return false;
	}


	void FiberDataIoAlgorithm::compute()
	{
		m_fail = true;
		if (m_location.empty())
		{
			LOG_ERROR("Filename empty");
			m_status = Status::InvalidInput;
			return;
		}

		if (m_ioMode == Read)
		{
			m_output = readTrkFile(m_location);
			m_fail = false;
			m_status = Status::Success;
		}
		else if (m_ioMode == Write && m_input != nullptr)
		{
			bool ok = saveTrkFile(*m_input, m_location);
			if (!ok)
				return;
		}
		m_fail = false;
		m_status = Status::Success;
	}


	OwningDataList FiberDataIoAlgorithm::takeOutput()
	{
		return OwningDataList(std::move(m_output));
	}


	std::vector<std::string> FiberDataIoAlgorithm::supportedFileExtensions() const
	{
		return {"trk"};
	}


	std::unique_ptr<ImFusion::FiberData> FiberDataIoAlgorithm::readTrkFile(const Filesystem::Path& filename) const
	{
		IMFUSION_ASSERT(sizeof(trk_header) == 1000);
		trk_header header;

		std::ifstream file(Platform::widen(filename), std::ios::in | std::ios::binary);
		if (!file)
		{
			LOG_ERROR("Failed to open file: " << filename);
			return nullptr;
		}
		if (!file.read((char*)&header, sizeof(header)))
		{
			LOG_ERROR("Failed to read header! File: " << filename);
			return nullptr;
		}

		auto toReturn = std::make_unique<FiberData>();
		toReturn->setName(filename.fullBaseName());

		mat4f matToWorld;
		static_assert(sizeof(matToWorld) == 64);
		std::memcpy(matToWorld.data(), header.vox_to_ras, 64);
		toReturn->setMatrixToWorld(matToWorld.cast<double>());

		int32_t numPoints;
		std::vector<float> scalars(header.n_scalars, 0.f);
		while (file.read(reinterpret_cast<char*>(&numPoints), 4))
		{
			std::vector<vec4f> vertices(numPoints, vec4f::Zero());
			for (int32_t i = 0; i < numPoints; ++i)
			{
				if (header.n_scalars == 1)
				{
					file.read(reinterpret_cast<char*>(vertices[i].data()), sizeof(vec4f));
				}
				else
				{
					vec3f vertex;
					file.read(reinterpret_cast<char*>(vertex.data()), sizeof(vec3f));
					vertices[i] << vertex, 0.f;
					if (header.n_scalars > 0)
						file.read(reinterpret_cast<char*>(scalars.data()), sizeof(float) * header.n_scalars);
				}

				vertices[i].head<3>() = vertices[i].head<3>().cwiseProduct(p_scaling.value());
				vertices[i].head<3>() += p_offset.value();
			}

			toReturn->addFiber(vertices);
		}
		file.close();

		return toReturn;
	}


	bool FiberDataIoAlgorithm::saveTrkFile(const FiberData& fibers, const Filesystem::Path& filename) const
	{
		std::ofstream file(Platform::widen(filename), std::ios::out | std::ios::binary);
		if (!file)
		{
			LOG_ERROR("Failed to open file: " << filename);
			return false;
		}

		trk_header header{};
		std::memset(&header, 0, sizeof(trk_header));
		strncpy_s(header.id_string, 6, "TRACK", 6);
		header.n_count = fibers.size();
		header.n_scalars = 1;
		strncpy_s(header.scalar_name[0], 20, "Anisotropy", 20);
		header.version = 2;
		mat4f matToWorld = fibers.matrixToWorld().cast<float>();
		static_assert(sizeof(matToWorld) == 64);
		std::memcpy(header.vox_to_ras, matToWorld.data(), 64);
		header.hdr_size = sizeof(trk_header);
		file.write(reinterpret_cast<const char*>(&header), sizeof(trk_header));

		const std::vector<vec4f>& vertices = fibers.vertices();
		for (const FiberData::Fiber& f : fibers.fibers())
		{
			int32_t numPoints = static_cast<int32_t>(f.endIndex - f.startIndex);
			file.write(reinterpret_cast<const char*>(&numPoints), sizeof(int32_t));

			for (size_t i = f.startIndex; i < f.endIndex; ++i)
				file.write(reinterpret_cast<const char*>(vertices[i].data()), 16);
		}
		if (!file)
		{
			LOG_ERROR("Error while writing fibers to: " << filename);
			return false;
		}
		file.close();
		return true;
	}

}
