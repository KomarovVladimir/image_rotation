#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <math.h>
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

		bool flip = true;
		if (info.height < 0)
		{
			flip = false;
			info.height = -info.height;
		}

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

			if (flip)
			{
				pixels.insert(pixels.begin(), rowData);
			}
			else
			{
				pixels.push_back(rowData);
			}
		}

		file.close();
	}
}

void saveBmp(std::string filename, PixelMatrix& pixels)
{
	std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);

	if (file.fail())
	{
		std::cerr << filename << " could not be opened for editing. "
			<< "Is it already open by another program or is it read-only?\n";
	}

	else
	{
		uchar_t bmpFileId[BMP_ID];
		bmpFileId[0] = 'B';
		bmpFileId[1] = 'M';
		file.write((char*)(&bmpFileId), sizeof(bmpFileId));
		bmpfile_header header = { 0 };
		header.bmp_offset = sizeof(bmpFileId)
			+ sizeof(bmpfile_header) + sizeof(bmpfile_dib_info);
		header.file_size = header.bmp_offset
			+ (pixels.size() * 3 + pixels[0].size() % 4) * pixels.size();
		file.write((char*)(&header), sizeof(header));
		bmpfile_dib_info dib_info = { 0 };
		dib_info.header_size = sizeof(bmpfile_dib_info);
		dib_info.width = pixels[0].size();
		dib_info.height = pixels.size();
		dib_info.num_planes = 1;
		dib_info.bits_per_pixel = 24;
		dib_info.compression = 0;
		dib_info.bmp_byte_size = 0;
		dib_info.hres = 2835;
		dib_info.vres = 2835;
		dib_info.num_colors = 0;
		dib_info.num_important_colors = 0;
		file.write((char*)(&dib_info), sizeof(dib_info));

		for (int row = pixels.size() - 1; row >= 0; row--)
		{
			const std::vector <Pixel>& row_data = pixels[row];

			for (int col = 0; col < row_data.size(); col++)
			{
				const Pixel& pix = row_data[col];

				file.put((uchar_t)(pix.blue));
				file.put((uchar_t)(pix.green));
				file.put((uchar_t)(pix.red));
			}

			for (int i = 0; i < row_data.size() % 4; i++)
			{
				file.put(0);
			}
		}

		file.close();
	}
}

void rotateImage(double deg, PixelMatrix& pixels) {
	const int height = pixels.size();
	const int width = pixels[0].size();
	const int offsetY = height / 2;
	const int offsetX = width / 2;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			/*int dx = (x + offsetX) * cos(deg) - (y + offsetY) * sin(deg);
			int dy = (x + offsetX) * sin(deg) + (y + offsetY) * cos(deg);*/

			int dx = x * cos(deg) - y * sin(deg);
			int dy = x * sin(deg) + y * cos(deg);

			if (dx > 0 && dx < width && dy > 0 && dy < height) {
				std::swap(pixels[y][x], pixels[dy][dx]);
				//pixels[y][x] = Pixel(0, 0, 0);
			}
		}
	}
}

int main()
{
	std::string input = "samples/FLAG_B24.BMP";
	std::string output = "results/FLAG_B24_rotated.bmp";

	PixelMatrix pixels;

	loadBmp(input, pixels);
	rotateImage(180, pixels);
	saveBmp(output, pixels);

	return 0;
}