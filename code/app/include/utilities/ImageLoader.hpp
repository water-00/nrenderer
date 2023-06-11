#pragma once
#ifndef __NR_IMAGE_LOADER_HPP__
#define __NR_IMAGE_LOADER_HPP__

#include <string>

#include "geometry/vec.hpp"
#include "Image.hpp"

namespace NRenderer {
	using namespace std;
	class ImageLoader
	{
	public:
		ImageLoader() = default;
		~ImageLoader() = default;
		Image* load(const string& file, int channel = 4);
	};
}

#endif