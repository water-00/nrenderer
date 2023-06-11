#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utilities/ImageLoader.hpp"

namespace NRenderer
{
	Image* ImageLoader::load(const string& file, int channel) {
		if (channel != 3 && channel != 4) return nullptr;
		Image* image = new Image();
		auto data = stbi_load(file.c_str(), &(image->width), &(image->height), &(image->channel), channel);
		image->channel = channel;
		image->data = new float[image->width * image->height * image->channel];
		for (unsigned long i = 0; i < image->height * image->width * channel; i++) {
			image->data[i] = (float)(data[i]) / 255.f;
		}
		stbi_image_free(data);
		return image;
	}
}