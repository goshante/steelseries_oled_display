#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <thread>
#include <map>

#include <hidapi.h>
#include "Canvas.h"

using namespace std::chrono_literals;

using pixel_t = unsigned char;
using bitmap_t = std::vector<std::vector<pixel_t>>;

bitmap_t CreateBitmap(size_t w, size_t h)
{
	bitmap_t bmp(w);
	for (auto& y : bmp)
		y.resize(h);
	return bmp;
}

template<typename T>
T randomBetween(T a, T b)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<T> uid(a, b);
	return uid(gen);
}

int GetFontWidth(bool doubleWidth = false)
{
	if (doubleWidth)
		return 16;
	return 8;
}

bitmap_t getPicture(const std::string& sign)
{
	auto SerializeBitmap = [](bitmap_t& canvas, int width, int height) -> std::vector<unsigned char>
	{
		//Export
		std::vector<pixel_t> buffer;
		buffer.resize(((width * height) / 8) + 2);
		int pos = 0;
		unsigned char posMinor = 0;
		int canvasPos = 0;

		//Write bitmap device command
		buffer[0] = 0x0;
		buffer[1] = 0x65;

		//Transform image
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (canvas[x][y] > 0)
					buffer[pos + 2] |= (1UL << (7 - posMinor));
				posMinor++;
				canvasPos++;
				if (posMinor > 7)
				{
					posMinor = 0;
					pos++;
				}
			}
		}

		return buffer;
	};

	//Actual bitmap
	const size_t w = 128, h = 40;
	auto bitmap = CreateBitmap(w, h);

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
			bitmap[x][y] = randomBetween<int>(0, 1);
	}

	return bitmap;//SerializeBitmap(bitmap, w, h);
}

int main()
{
	Canvas canvas(128, 40, "Preview", 0xFFFFFFFF);
	HIDDevice device;
	const std::string devName = "VID_1038&PID_1612&mi_01";
	device.Open(devName, true);

	
	
	if (!device.Open(devName, true))
	{
		std::cout << device.GetLastErrorStrA() << std::endl;
		return 1;
	}

	while (true)
	{
		auto pic = getPicture("Hello");
		/*if (device.SendFeatureReport(pic) == -1)
		{
			std::cout << "SendFeatureReport failed: " << device.GetLastErrorStrA() << std::endl;
			return 3;
		}*/
		canvas.Push(pic);
		std::this_thread::sleep_for(10ms);
	}

	std::cout << device.GetManufacturerA() << " " << device.GetProductStrA() << " " << device.GetSerialA() << std::endl;

	device.Close();
	return 0;
}