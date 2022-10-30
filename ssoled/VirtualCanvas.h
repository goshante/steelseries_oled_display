#pragma once
#include "defs.h"
#include "Font.h"

class VirtualCanvas
{
public:
	struct Dims
	{
		int a_x;
		int a_y;
		int b_x;
		int b_y;
	};

private:
	bitmap_t	_canvas;
	int			_width;
	int			_height;

	VirtualCanvas(VirtualCanvas&) = delete;
	VirtualCanvas& operator=(VirtualCanvas&) = delete;

public:
	VirtualCanvas(int w, int h);
	~VirtualCanvas();

	bitmap_t GetBitmap() const;
	std::vector<unsigned char> GetSerializedData() const;

	Dims DrawTextRegular(const Font& font, const std::string& text, int off_x, int off_y, int brush = 1, bool invert = false, bool monospace = false);
	Dims DrawTextUTF8(const Font& font, const std::string& text, int off_x, int off_y, int brush = 1, bool invert = false, bool monospace = false);
	void DrawLine(int ax, int ay, int bx, int by);
	void Clear();
};