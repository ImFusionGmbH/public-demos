/* Copyright (c) 2012-2019 ImFusion GmbH, Munich, Germany. All rights reserved. */
#pragma once

#include <memory>

#include "itkImage.h"
#include "itkRGBPixel.h"

namespace ImFusion
{
	class MemImage;
	class Image;
	template <typename T>
	class TypedImage;

	/** \brief	Conversion between ImFusion and ITK image containers
	*/
	namespace ITK
	{
		typedef itk::RGBPixel<double> RGBPixelType;
		typedef itk::Image<RGBPixelType, 2> RGBImageType;
		typedef itk::Image<double, 2> RealImageType;

		/// Convert ImFusion image to ITK image
		RGBImageType::Pointer convert(const TypedImage<double>& img);

		/// Convert ITK image to ImFusion image
		std::unique_ptr<TypedImage<double>> convert(RGBImageType::Pointer& image);
	}
}
