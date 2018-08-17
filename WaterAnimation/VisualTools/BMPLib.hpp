#ifndef BMPLIB
#define BMPLIB


/*
stb single-file public domain libraries for C/C++
https://github.com/nothings/stb
*/

#include "stdafx.h"

// only using the bmp format
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>


using byte = unsigned char;

struct  RGBManip {
	byte &R;
	byte &G;
	byte &B;
	//note that BMP store information in a reversed order
	RGBManip(byte *first) :B{ *(first) }, G{ *(first + 1) }, R{ *(first + 2) } {}
	RGBManip(const RGBManip &other) :B{ other.B }, G{ other.G }, R{ other.R } {}
};

static void raise(const char* err_info) {
	cerr << err_info << "\n";
	throw runtime_error(err_info);
}

//Stores information of a given BMP file.
//The raw image data is stored in an 1d array.
//Only accept true color image.

struct  BMPInfo {
	// initialize an BMPInfo object with blank array
	BMPInfo(int w, int h) :width{ w }, height{ h }, datasize{ 3 * w * h }, ptr{ new byte[datasize] } {
		if (w < 1)raise(" the width of image is too small.");
		if (h < 1)raise("the height of image is too small.");
	}

	// initialize an BMPInfo object with a raw pointer to image data.
	BMPInfo(int w, int h, byte *raw) :ptr{ raw }, width{ w }, height{ h } {
		if (w < 1)raise(" the width of image is too small.");
		if (h < 1)raise("the height of image is too small.");
		datasize = 3 * width * height;
	}

	BMPInfo(const BMPInfo &other) = delete;

	BMPInfo &operator = (const BMPInfo &other) = delete;

	BMPInfo(BMPInfo &&other) {
		*this = move(other);
	}

	BMPInfo &operator = (BMPInfo &&other) {
		width = other.width;
		height = other.height;
		datasize = other.datasize;
		ptr = move(other.ptr);
		return *this;
	}

	~BMPInfo() {
		delete ptr;
	}

	int getWidth() { return width; }
	int getHeight() { return height; }
	int getDatasize() { return datasize; }

	// access the raw pointer itself. Please do not delete the pointer returned.
	byte* getPtr() { return ptr; }

	// copy image information from another identical datasize BMPInfo
	void copyFrom(BMPInfo &other) {
		if (datasize != other.datasize)raise("cannot copy from a different datasize BMPInfo");
		for (int i = 0; i < datasize; ++i) {
			ptr[i] = other.ptr[i];
		}
	}


	// The data stored in the array is inverted.
	RGBManip operator()(int w, int h) {
		if (w >= width || w < 0)raise("width index incorrect.");
		if (h >= height || h < 0)raise("height index incorrect.");
		int n = getFirstElementPointer(width - 1 - w, height - 1 - h);
		return RGBManip{ ptr + n };
	}

private:
	int getFirstElementPointer(int w, int h) {
		return 3 * (h * width + w);
	}
	int width = 0;
	int height = 0;
	int datasize = 0;
	byte* ptr = nullptr;

};



//Only 24bit true color image is supported.
inline BMPInfo BMPLoad(const string &filename){
	int w, h, channel;
	byte* pointer = stbi_load(filename.c_str(), &w, &h, &channel, 3);
	if (channel != 3) {
		raise("only 24 bit images are supported.");
	}
	return BMPInfo{ w,h,pointer };
}

inline void BMPSave(BMPInfo &info, const string &filename){
	stbi_write_bmp(filename.c_str(), info.getWidth(), info.getHeight(), 3, info.getPtr());
}

// color conversion
inline  byte float2byte(float clr) {
	if (clr > 1.0f)return 255;
	if (clr < 0.0f)return 0;
	return (byte)(roundf(clr * 255.0f));
}

inline  float byte2float(byte clr) {
	return (float)clr / 255.0f;
}




// set back to default alignment
#pragma pack()

#endif BMPLIB
