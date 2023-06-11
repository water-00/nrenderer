#pragma once
#ifndef __NR_IMAGE_HPP__
#define __NR_IMAGE_HPP__

namespace NRenderer
{
    using namespace std;
	struct Image {
		int height;
		int width;
		int channel;
		// range: 0.0 - 1.0
		float* data;
		Image()
			: height(0)
			, width(0)
			, channel(0)
			, data(nullptr)
		{}
        Image(const Image& image) {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = new float[size];
            for (int i = 0; i < size; i++) {
                this->data[i] = image.data[i];
            }
        }
        Image(Image&& image) noexcept {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = image.data;
            image.data = nullptr;
        }
        ~Image() {
            delete[] data;
        }
	};
}

#endif