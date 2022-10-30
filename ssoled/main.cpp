#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <thread>
#include <map>

#include <hidapi.h>
#include "Canvas.h"
#include "VirtualCanvas.h"
#include "Font.h"
#include <windows.ui.notifications.h>

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
	try
	{
		Canvas canvas(128, 40, "Preview", 0xFFFFFFFF);
		VirtualCanvas vc(128, 40);
		Font font1("font1.fnt");
		Font font2("smol.fnt");
		HIDDevice device;
		const std::string devName = "VID_1038&PID_1612&mi_01";
		device.Open(devName, true);

		/*if (!device.Open(devName, true))
		{
			std::cout << device.GetLastErrorStrA() << std::endl;
			return 1;
		}*/

		int i = 0;
		while (true)
		{
			vc.DrawTextRegular(font1, "Messages:", 3, 0);
			vc.DrawTextRegular(font2, "TEST: EEEEEEEEEEE", 5, 14);
			vc.DrawTextRegular(font2, "Почему бы бибу не соснуть?", 5-i, 14+ 9);
			vc.DrawTextRegular(font2, "TEST 33333333333", 5, 14 + 9 * 2);
			vc.DrawLine(2, 8, 100, 35);
			auto frame = vc.GetBitmap();
			vc.Clear();
			/*if (device.SendFeatureReport(pic) == -1)
			{
				std::cout << "SendFeatureReport failed: " << device.GetLastErrorStrA() << std::endl;
				return 3;
			}*/
			canvas.Push(frame);
			std::this_thread::sleep_for(60ms);
			i++;
			if (i >= 127)
				i = -127;
		}

		std::cout << device.GetManufacturerA() << " " << device.GetProductStrA() << " " << device.GetSerialA() << std::endl;

		device.Close();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}
	return 0;
}