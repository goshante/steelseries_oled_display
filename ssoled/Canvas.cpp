#include "Canvas.h"

#define min(a,b) (a < b ? a : b)

Canvas::Canvas(int w, int h, const std::string& title, uint32_t brush, uint32_t background)
	: _pw(nullptr)
	, _title(title)
	, _width(w)
	, _height(h)
	, _background(background)
	, _brush(brush)
	, _paused(false)
{
	_frame = std::vector<std::vector<pixel_t>>(_width);
	for (auto& x : _frame)
	{
		x = std::vector<pixel_t>(_height);
		for (auto& y : x)
			y = 0;
	}
	_thread = std::make_shared<std::thread>(&Canvas::_draw, this);
}

Canvas::~Canvas()
{
	Show(false);
}

void Canvas::Push(const bitmap_t& picture)
{
	_lock.lock();
	_frame = picture;
	_lock.unlock();
}

void Canvas::Show(bool show)
{
	if (!show)
	{
		_paused = true;
		_thread->join();
	}
	else
	{
		_paused = false;
		_thread = std::make_shared<std::thread>(&Canvas::_draw, this);
	}
}

void Canvas::_draw()
{
	_pw = std::make_shared<pw::PixelWindow>(_width, _height, _title.c_str());
	while (!_paused)
	{
		_pw->makeCurrent();
		_pw->pollEvents();
		_pw->beginFrame();
		_pw->setBackgroundColor(_background);
		_lock.lock();
		for (int x = 0; x < min(_width, _frame.size()); x++)
		{
			for (int y = 0; y < min(_height, _frame[x].size()); y++)
			{
				if (_frame[x][y] > 0)
					_pw->setPixel(x, y, _brush);
			}
		}
		_lock.unlock();
		_pw->endFrame();
	}
	_pw->forceClose();
}