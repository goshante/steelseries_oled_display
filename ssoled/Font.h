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

public:
	Font(const std::string& pathToTxtFont, int h, int w, const std::string& seq = "");
	Font(std::vector<unsigned char> dict, int h, int w, const std::string& seq = "");
	Font(const Font& copy);
	Font& operator=(const Font& copy);
	~Font();

	//Each pixel is a byte
	bitmap_t GetLetterImage_8bit(utf8char_t ch);

	//Each pixel is a bit
	std::vector<unsigned char> GetLetterImage_1bit(utf8char_t ch);
};