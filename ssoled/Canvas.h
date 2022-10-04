#pragma once
#include <vector>
#include <string>
#include <PixelWindow/PixelWindow.h>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>

#include "defs.h"

class Canvas
{
private:
	std::shared_ptr<pw::PixelWindow>	_pw;
	std::shared_ptr<std::thread>		_thread;
	std::string							_title;
	std::mutex							_lock;
	bitmap_t							_frame;
	int									_width;
	int									_height;
	uint32_t							_background;
	uint32_t							_brush;
	std::atomic<bool>					_paused;

	Canvas(Canvas&) = delete;
	Canvas& operator=(Canvas&) = delete;

	void _draw();

public:
	Canvas(int w, int h, const std::string& title = "Preview", uint32_t brush = 0xFFFFFF00, uint32_t background = 0x00000000);
	~Canvas();

	void Push(const bitmap_t& picture);
	void Show(bool show);
};