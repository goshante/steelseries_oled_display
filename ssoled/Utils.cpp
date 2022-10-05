#include "Utils.h"

void InitBitmap(bitmap_t& bmp, int h, int w)
{
	bmp.clear();
	bmp.resize(h);
	for (auto& y : bmp)
	{
		y.resize(w);
		for (auto& x : y)
			x = 0;
	}
}

void RemoveBOMFromString(std::string& str)
{
	if (str.length() < 3)
		return;

	if (str[0] == 0xEF && str[1] == 0xBB && str[2] == 0xBF)
		str = str.substr(3, str.length() - 1);
}

size_t strlen_utf8(const std::string& u8str)
{
	size_t len = 0;
	for (size_t i = 0; i < u8str.length();)
	{
		int cplen = 1;
		if ((u8str[i] & 0xf8) == 0xf0) cplen = 4;
		else if ((u8str[i] & 0xf0) == 0xe0) cplen = 3;
		else if ((u8str[i] & 0xe0) == 0xc0) cplen = 2;
		if ((i + cplen) > u8str.length()) cplen = 1;

		len++;
		i += cplen;
	}

	return len;
}

void enumerateUTF8String(const std::string& u8str, std::function<void(utf8char_t ch, size_t n, size_t cpsz)> callback)
{
	size_t n = 0;
	for (size_t i = 0; i < u8str.length();)
	{
		int cplen = 1;
		if ((u8str[i] & 0xf8) == 0xf0) cplen = 4;
		else if ((u8str[i] & 0xf0) == 0xe0) cplen = 3;
		else if ((u8str[i] & 0xe0) == 0xc0) cplen = 2;
		if ((i + cplen) > u8str.length()) cplen = 1;
		utf8char_t ch = 0;
		auto sch = u8str.substr(i, cplen);
		for (size_t k = 0; k < cplen; k++)
			reinterpret_cast<char*>(&ch)[k] = sch[k];
		callback(ch, n, cplen);
		n++;
		i += cplen;
	}
}

std::string utf8char_to_stdString(utf8char_t ch)
{
	std::string str;
	size_t len = 0;
	char* u8str = (char*)&ch;
	int cplen = 1;
	if ((u8str[0] & 0xf8) == 0xf0) cplen = 4;
	else if ((u8str[0] & 0xf0) == 0xe0) cplen = 3;
	else if ((u8str[0] & 0xe0) == 0xc0) cplen = 2;

	for (size_t i = 0; i < cplen; i++)
		str.push_back(u8str[i]);

	return str;
}