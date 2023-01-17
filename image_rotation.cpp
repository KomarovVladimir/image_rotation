#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <cmath>
#include "bitmap.h"

bool rotate(std::vector <std::vector <Pixel> >& bmp, int angle)
{
	int height = bmp.size() / 100;
	int width = bmp.size() / 100;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int dx = i - width / 2;
			int dy = j - height / 2;
			int nx = cos(angle) * dx + sin(angle) * dx;
			int ny = cos(angle) * dy - sin(angle) * dy;
			std::swap(bmp[i][j], bmp[ny][nx]);
		}
	}

	return true;
}

int main()
{
	std::string input2 = "samples/FLAG_B24.BMP";
	std::string input = "samples/lena_color.bmp";
	std::string output = "results/lena_color_rotated.bmp";

	Bitmap image;
	std::vector <std::vector <Pixel> > bmp;
	Pixel rgb;

	//read a file example.bmp and convert it to a pixel matrix
	image.open(input);

	//verify that the file opened was a valid image
	bool validBmp = image.isImage();

	if (validBmp == true)
	{
		bmp = image.toPixelMatrix();

		rotate(bmp, 45);

		image.fromPixelMatrix(bmp);
		image.save(output);
	}

	return 0;
}