#pragma once
#include "defs.h"
#include <string>
#include <functional>

#define Min(a,b) (a < b ? a : b)
#define Max(a,b) (a > b ? a : b)

void InitBitmap(bitmap_t& bmp, int h, int w);
void RemoveBOMFromString(std::string& str);
size_t strlen_utf8(const std::string& u8str);
void enumerateUTF8String(const std::string& u8str, std::function<void(utf8char_t ch, size_t n, size_t cpsz)> callback);
std::string utf8char_to_stdString(utf8char_t ch);