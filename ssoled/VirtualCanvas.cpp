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

VirtualCanvas::Dims VirtualCanvas::DrawTextRegular(const Font& font, const std::string& text, int off_x, int off_y, int brush, bool invert, bool monospace)
{
	int fh = font.GetHeight();
	Dims dims;
	dims.a_x = off_x;
	dims.a_y = off_y;
	dims.b_x = off_x;
	dims.b_y = off_y + fh;
	int initialX = off_x;

	int cur_row_w = off_x;
	for (size_t i = 0; i < text.length(); i++)
	{
		char c = text[i];
		auto letter = font.GetCharImage_8bit((unsigned char)c, monospace);

		if (letter.empty())
			continue;

		int fw = (int)letter[0].size();

		if (c == '\r')
			continue;

		if (c == '\n')
		{
			off_y += 1 + fh;
			dims.b_y += 1 + fh;


			if (cur_row_w > dims.b_x)
				dims.b_x = cur_row_w;

			off_x = initialX;
			cur_row_w = initialX;
			continue;
		}

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
					_canvas[pos_y][pos_x] = brush;
			}
		}
		off_x += fw + font.GetInterval();
		cur_row_w += fw + font.GetInterval();
	}

	if (dims.b_x == initialX)
		dims.b_x = cur_row_w;
	return dims;
}

VirtualCanvas::Dims VirtualCanvas::DrawTextUTF8(const Font& font, const std::string& text, int off_x, int off_y, int brush, bool invert, bool monospace)
{
	int fh = font.GetHeight();
	Dims dims;
	dims.a_x = off_x;
	dims.a_y = off_y;
	dims.b_x = off_x;
	dims.b_y = off_y + fh;
	int initialX = off_x;

	int cur_row_w = off_x;
	enumerateUTF8String(text, [&](utf8char_t c, size_t i, size_t cpsz)
	{
		auto letter = font.GetCharImage_8bit(c, monospace);

		if (letter.empty())
			return;

		int fw = (int)letter[0].size();

		if (c == '\r')
			return;

		if (c == '\n')
		{
			off_y += 1 + fh;
			dims.b_y += 1 + fh;


			if (cur_row_w > dims.b_x)
				dims.b_x = cur_row_w;

			off_x = initialX;
			cur_row_w = initialX;
			return;
		}

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
					_canvas[pos_y][pos_x] = brush;
			}
		}
		off_x += fw + font.GetInterval();
		cur_row_w += fw + font.GetInterval();
	});

	if (dims.b_x == initialX)
		dims.b_x = cur_row_w;
	return dims;
}

void VirtualCanvas::DrawLine(int ax, int ay, int bx, int by)
{
	
}

void VirtualCanvas::Clear()
{
	InitBitmap(_canvas, _height, _width);
}