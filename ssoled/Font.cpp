#include "Font.h"

Font::Font(const std::string& pathToTxtFont, int h, int w, const std::string& seq)
{

}

Font::Font(std::vector<unsigned char> dict, int h, int w, const std::string& seq)
{

}

Font::Font(const Font& copy)
{

}

Font& Font::operator=(const Font& copy)
{
	return *this;
}

Font::~Font()
{

}

//Each pixel is a byte
bitmap_t Font::GetLetterImage_8bit(utf8char_t ch)
{
	return {};
}

//Each pixel is a bit
std::vector<unsigned char> Font::GetLetterImage_1bit(utf8char_t ch)
{
	return {};
}