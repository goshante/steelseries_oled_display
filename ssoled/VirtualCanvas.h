#pragma once
#include "defs.h"
#include "Font.h"

class VirtualCanvas
{
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

	void DrawTextRegular(const Font& font, const std::string& text, int off_x, int off_y, bool invert = false);
	void DrawTextUTF8(const Font& font, const std::string& text, int off_x, int off_y, bool invert = false);
	void DrawLine(int ax, int ay, int bx, int by);
	void Clear();
};