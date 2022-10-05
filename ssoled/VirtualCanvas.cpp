#pragma once
#include "VirtualCanvas.h"
#include "Utils.h"
#include <cmath>

#define mod(n) (n < 0 ? n * -1 : n)

VirtualCanvas::VirtualCanvas(int w, int h)
	: _width(w)
	, _height(h)
{
	InitBitmap(_canvas, h, w);
}

VirtualCanvas::~VirtualCanvas()
{
}

bitmap_t VirtualCanvas::GetBitmap() const
{
	return _canvas;
}

std::vector<unsigned char> VirtualCanvas::GetSerializedData() const
{
	std::vector<pixel_t> buffer;
	buffer.resize(((_width * _height) / 8) + 2);
	int pos = 0;
	unsigned char posMinor = 0;
	int canvasPos = 0;

	//Write bitmap device command
	buffer[0] = 0x0;
	buffer[1] = 0x65;

	//Transform image
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			if (_canvas[y][x] > 0)
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
}

void VirtualCanvas::DrawTextRegular(const Font& font, const std::string& text, int off_x, int off_y, bool invert)
{
	int fw = font.GetWidth();
	int fh = font.GetHeight();
	for (size_t i = 0; i < text.length(); i++)
	{
		char c = text[i];
		auto letter = font.GetLetterImage_8bit(c);
		if (letter.empty())
			continue;

		for (int y = 0; y < fh; y++)
		{
			for (int x = 0; x < fw; x++)
			{
				int pos_x = x + off_x;
				int pos_y = y + off_y;
				if (pos_x >= _width || pos_y >= _height || pos_x < 0 || pos_y < 0)
					break;

				if ((!invert && letter[y][x] == 1) ||
					(invert && letter[y][x] == 0))
					_canvas[pos_y][pos_x] = 1;
			}
		}
		off_x += fw;
	}
}

void VirtualCanvas::DrawTextUTF8(const Font& font, const std::string& text, int off_x, int off_y, bool invert)
{
	int fw = font.GetWidth();
	int fh = font.GetHeight();
	enumerateUTF8String(text, [&](utf8char_t c, size_t n, size_t cpsz)
	{
		auto letter = font.GetLetterImage_8bit(c);
		if (letter.empty())
			return;

		for (int y = 0; y < fh; y++)
		{
			for (int x = 0; x < fw; x++)
			{
				int pos_x = x + off_x;
				int pos_y = y + off_y;
				if (pos_x >= _width || pos_y >= _height || pos_x < 0 || pos_y < 0)
					break;

				if ((!invert && letter[y][x] == 1) ||
					(invert && letter[y][x] == 0))
					_canvas[pos_y][pos_x] = 1;
			}
		}
		off_x += fw;
	});
}

void VirtualCanvas::DrawLine(int ax, int ay, int bx, int by)
{
	int dx = bx - ax;
	int dy = by - ay;

	_canvas[ay][ax] = 1;
	if (dx != 0)
	{
		int m = dy / dx;
		int b = ay - m * ax;
		if (bx > ax)
			dx = 1;
		else
			dx = -1;
		while (ax != bx)
		{
			ax = ax + dx;
			ay = floor(m * ax + b + 0.5);
			_canvas[ay][ax] = 1;
		}
	}
}

void VirtualCanvas::Clear()
{
	InitBitmap(_canvas, _height, _width);
}