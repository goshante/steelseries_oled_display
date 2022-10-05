#include "Font.h"
#include <Windows.h>
#include <stdexcept>
#include "reutils.h"
#include "Utils.h"

Font::Font(const std::string& pathToTxtFont, const std::string& seq, bool utf8)
	: _seq(seq)
	, _utf8(utf8)
{
	HANDLE hFile = CreateFileA(pathToTxtFont.c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Failed to open font file");

	DWORD dwWritten = 0, dwSize = GetFileSize(hFile, NULL);
	char* buf = new char[dwSize + 1];
	if (!ReadFile(hFile, buf, dwSize, &dwWritten, NULL))
	{
		CloseHandle(hFile);
		throw std::runtime_error("Failed to read file");
	}
	CloseHandle(hFile);
	buf[dwSize] = '\0';

	std::string fileContent = buf;
	delete[] buf;
	RemoveBOMFromString(fileContent);
	auto match = reu::Search(fileContent, "^([0-9]+)x([0-9]+)\\n");
	if (!match.IsMatching())
		throw std::runtime_error("Font file has invalid format");

	_width = std::atoi(match[1].c_str());
	_height = std::atoi(match[2].c_str());

	for (size_t i = match.End() + 1; i < fileContent.length(); i++)
		_dict.push_back(fileContent[i] == '0' ? 0 : 1);

	if (_dict.size() % (_width * _height) != 0)
		throw std::runtime_error("Font is corrupted or has wrong resolution");

	auto count = _dict.size() / (_width * _height);
	if (seq.empty() && count != 256)
		throw std::runtime_error("Font character count mismatch, expected full ASCII representation");

	if (utf8)
	{
		if (!seq.empty() && count != strlen_utf8(seq))
			throw std::runtime_error("Font character count mismatch");
	}
	else
	{
		if (!seq.empty() && count != seq.length())
			throw std::runtime_error("Font character count mismatch");
	}
}

Font::Font(const std::vector<unsigned char>& dict, int h, int w, const std::string& seq, bool utf8)
	: _dict(dict)
	, _height(h)
	, _width(w)
	, _seq(seq)
	, _utf8(utf8)
{
	if (_dict.size() % (_width * _height) != 0)
		throw std::runtime_error("Font is corrupted or has wrong resolution");

	auto count = _dict.size() / (_width * _height);
	if (seq.empty() && count != 256)
		throw std::runtime_error("Font character count mismatch, expected full ASCII representation");

	if (utf8)
	{
		if (!seq.empty() && count != strlen_utf8(seq))
			throw std::runtime_error("Font character count mismatch");
	}
	else
	{
		if (!seq.empty() && count != seq.length())
			throw std::runtime_error("Font character count mismatch");
	}
}

Font::Font(const Font& copy)
	: _dict(copy._dict)
	, _height(copy._height)
	, _width(copy._width)
	, _seq(copy._seq)
	, _utf8(copy._utf8)
{
}

Font& Font::operator=(const Font& copy)
{
	_dict = copy._dict;
	_height = copy._height;
	_width = copy._width;
	_seq = copy._seq;
	_utf8 = copy._utf8;
	return *this;
}

Font::~Font()
{
}

int Font::GetHeight() const
{
	return _height;
}

int Font::GetWidth() const
{
	return _width;
}

//Each pixel is a byte
bitmap_t Font::GetLetterImage_8bit(utf8char_t ch) const
{
	bitmap_t letter;
	InitBitmap(letter, _height, _width);
	size_t letterSize = _width * _height;

	if (_seq.empty())
	{
		bool skip_retry = false;
	noseq_search:
		for (unsigned char c = 0; c < 256; c++)
		{
			if (c == (unsigned char)ch)
			{
				size_t q = 0;
				for (size_t y = 0; y < letter.size(); y++)
				{
					for (size_t x = 0; x < letter[y].size(); x++)
					{
						letter[y][x] = _dict[(c * letterSize) + q];
						q++;
					}
				}
				return letter;
			}
		}

		if (!skip_retry)
		{
			skip_retry = true;
			ch = '?';
			goto noseq_search;
		}

		return {};
	}

	if (_utf8)
	{
		bool found = false;
		bool skip_retry = false;
	utf8_enum:
		enumerateUTF8String(_seq, [&](utf8char_t uc, size_t n, size_t cpsz)
		{
			if (uc == ch)
			{
				size_t q = 0;
				for (size_t y = 0; y < letter.size(); y++)
				{
					for (size_t x = 0; x < letter[y].size(); x++)
					{
						letter[y][x] = _dict[(n * letterSize) + q];
						q++;
					}
				}
				found = true;
			}
		});
		if (found)
			return letter;
		

		if (!skip_retry)
		{
			ch = u8'?';
			skip_retry = true;
			goto utf8_enum;
		}

	}
	else
	{
		bool skip_retry = false;
	custom_seq_loop:
		for (size_t i = 0; i < _seq.length(); i++)
		{
			if (_seq[i] == (char)ch)
			{
				size_t q = 0;
				for (size_t y = 0; y < letter.size(); y++)
				{
					for (size_t x = 0; x < letter[y].size(); x++)
					{
						letter[y][x] = _dict[(i * letterSize) + q];
						q++;
					}
				}
				return letter;
			}
		}

		if (!skip_retry)
		{
			ch = '?';
			skip_retry = true;
			goto custom_seq_loop;
		}
	}

	return {};
}