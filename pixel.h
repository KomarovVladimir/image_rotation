#pragma once
class Pixel
{
public:
	int red, green, blue;

	Pixel() : red(0), green(0), blue(0) { }

	Pixel(int r, int g, int b) : red(r), green(g), blue(b) { }
};
