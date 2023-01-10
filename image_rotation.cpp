#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

bool rotate(char* src, char* dst, BITMAPINFOHEADER& bi, int angle)
{
	//In 24bit image, the length of each row must be multiple of 4
	int padw = 4 - ((bi.biWidth * 3) % 4);
	if (padw == 4) padw = 0;

	int padh = 4 - ((bi.biHeight * 3) % 4);
	if (padh == 4) padh = 0;

	int pad2 = 0;
	if (padh == 1 || padh == 3) pad2 = 2;

	bi.biHeight += padh;

	int w = bi.biWidth;
	int h = bi.biHeight;
	if (angle == 90 || angle == 270)
	{
		std::swap(bi.biWidth, bi.biHeight);
	}
	else
	{
		bi.biHeight -= padh;
	}

	for (int row = 0; row < h; row++)
	{
		for (int col = 0; col < w; col++)
		{
			int n1 = 3 * (col + w * row) + padw * row;
			int n2 = 0;

			switch (angle)
			{
			case 0:   n2 = 3 * (col + w * row) + padw * row; break;
			case 90:  n2 = 3 * ((h - row - 1) + h * col) + pad2 * col; break;
			case 180: n2 = 3 * (col + w * (h - row - 1)) + padw * (h - row - 1); break;
			case 270: n2 = 3 * (row + h * col) + pad2 * col; break;
			}

			dst[n2 + 0] = src[n1 + 0];
			dst[n2 + 1] = src[n1 + 1];
			dst[n2 + 2] = src[n1 + 2];
		}
	}

	for (int row = 0; row < bi.biHeight; row++)
		for (int col = 0; col < padw; col++)
			dst[bi.biWidth * 3 + col] = 0;

	bi.biSizeImage = (bi.biWidth + padw) * bi.biHeight * 3;

	return true;
}

int main()
{
	std::string input = "samples/lena_color.bmp";
	std::string output = "results/lena_color_rotated.bmp";

	BITMAPFILEHEADER bf = { 0 };
	BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER) };

	std::ifstream fin(input, std::ios::binary);
	if (!fin) return 0;

	fin.read((char*)&bf, sizeof(bf));
	fin.read((char*)&bi, sizeof(bi));

	int size = 3 * (bi.biWidth + 3) * (bi.biHeight + 3);
	char* src = new char[size];
	char* dst = new char[size];

	fin.read(src, bi.biSizeImage);

	//use 0, 90, 180, or 270 for the angle
	if (rotate(src, dst, bi, 90))
	{
		bf.bfSize = 54 + bi.biSizeImage;
		std::ofstream fout(output, std::ios::binary);
		fout.write((char*)&bf, 14);
		fout.write((char*)&bi, 40);
		fout.write((char*)dst, bi.biSizeImage);
	}

	delete[]src;
	delete[]dst;

	return 0;
}