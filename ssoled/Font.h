#pragma once
#include <vector>
#include <string>
#include "defs.h"

class Font
{
private:
	std::vector<unsigned char>		_dict;
	int								_height;
	int								_width;
	std::string						_seq;	//Empty associated char sequence means font is 0-255 ASCII representation
	bool							_utf8;

public:
	Font(const std::string& pathToTxtFont, const std::string& seq = "", bool utf8 = false);
	Font(const std::vector<unsigned char>& dict, int h, int w, const std::string& seq = "", bool utf8 = false);
	Font(const Font& copy);
	Font& operator=(const Font& copy);
	~Font();

	int GetHeight() const;
	int GetWidth() const;
	//Each pixel is a byte
	bitmap_t GetLetterImage_8bit(utf8char_t ch) const;
};