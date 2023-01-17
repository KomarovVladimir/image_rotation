#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <cmath>
#include "pixel.h"

const int BITS_PER_PIXEL = 24;
const int BMP_ID = 2;

typedef unsigned char uchar_t;
typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef signed short int int16_t;
typedef std::vector<std::vector<Pixel>> PixelMatrix;

struct bmpfile_header
{
	uint32_t file_size;
	uint16_t creator1;
	uint16_t creator2;
	uint32_t bmp_offset;
};

struct bmpfile_dib_info
{
	uint32_t header_size;
	int32_t width;
	int32_t height;
	uint16_t num_planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t bmp_byte_size;
	int32_t hres;
	int32_t vres;
	uint32_t num_colors;
	uint32_t num_important_colors;
};

//std::string input = "samples/lena_color.bmp";

void loadBmp(std::string fileName, PixelMatrix& pixels) {
	std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);

	if (file.fail()) {
		std::cerr << "Error: could not read the file." << std::endl;
	}

	uchar_t bmpFileId[BMP_ID];
	file.read((char*)(&bmpFileId), sizeof(bmpFileId));

	if (bmpFileId[0] != 'B' || bmpFileId[1] != 'M') {
		std::cerr << "Error: wrong format." << std::endl;
	}
	else {
		bmpfile_header header = { 0 };
		bmpfile_dib_info info = { sizeof(header) };

		file.read((char*)(&header), sizeof(header));
		file.read((char*)(&info), sizeof(info));

		if (info.bits_per_pixel != 24)
		{
			std::cerr << fileName << " uses " << info.bits_per_pixel
				<< "bits per pixel (bit depth). Bitmap only supports 24bit.\n";
		}

		if (info.compression != 0)
		{
			std::cerr << fileName << " is compressed. "
				<< "Bitmap only supports uncompressed images.\n";
		}

		file.seekg(header.bmp_offset);

		for (int row = 0; row < info.height; row++)
		{
			std::vector<Pixel> rowData;

			for (int col = 0; col < info.width; col++)
			{
				int blue = file.get();
				int green = file.get();
				int red = file.get();

				rowData.push_back(Pixel(red, green, blue));
			}

			file.seekg(info.width % 4, std::ios::cur);

			pixels.push_back(rowData);
		}

		file.close();
	}
}

int main()
{
	std::string input = "samples/FLAG_B24.BMP";
	std::string output = "results/lena_color_rotated.bmp";

	PixelMatrix pixels;

	loadBmp(input, pixels);

	for (int i = 0; i < pixels.size() / 10; i++)
	{
		for (int j = 0; j < pixels[i].size() / 10; j++)
		{
			std::cout << pixels[i][j].red << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "test";
	return 0;
}