#include "itkImageRegionIterator.h"

#include <ImFusion/Base/TypedImage.h>

#include <ITK.h>

namespace ImFusion
{
	namespace ITK
	{
		RGBImageType::Pointer convert(const TypedImage<double>& img)
		{
			RGBImageType::Pointer itkImage = RGBImageType::New();

			RGBImageType::SizeType size;
			size[0] = img.width();
			size[1] = img.height();
			RGBImageType::IndexType start;
			start[0] = 0;
			start[1] = 0;

			RGBImageType::RegionType region;
			region.SetSize(size);
			region.SetIndex(start);
			itkImage->SetRegions(region);
			itkImage->Allocate();
			double spacing[2];
			spacing[0] = img.spacing().x();
			spacing[1] = img.spacing().y();
			itkImage->SetSpacing(spacing);
			double origin[2];
			origin[0] = 0;
			origin[1] = 0;
			itkImage->SetOrigin(origin);

			typedef itk::ImageRegionIterator<RGBImageType> IteratorType;
			IteratorType it(itkImage, region);
			it.GoToBegin();
			const double* data = img.pointer();

			RGBPixelType pixel;
			while (!it.IsAtEnd())
			{
				for (unsigned int channel = 0; channel < pixel.Size(); ++channel)
				{
					pixel[channel] = *data;
					++data;
				}

				it.Set(pixel);
				++it;
			}

			return itkImage;
		}

		std::unique_ptr<TypedImage<double>> convert(RGBImageType::Pointer& image)
		{
			image->Update();
			RGBImageType::RegionType region;
			region = image->GetLargestPossibleRegion();
			itk::Size<2> size = region.GetSize();

			std::unique_ptr<TypedImage<double>> output = std::make_unique<TypedImage<double>>(vec3i(size[0], size[1], 1), 3);

			typedef itk::ImageRegionConstIterator<RGBImageType> IteratorType;
			IteratorType it(image, region);
			it.GoToBegin();
			double* data = output->pointer();
			while (!it.IsAtEnd())
			{
				RGBPixelType pixel;
				pixel = it.Get();
				for (unsigned int channel = 0; channel < pixel.Size(); ++channel)
				{
					*data = pixel[channel];
					++data;
				}
				++it;
			}

			return output;
		}

	}
}